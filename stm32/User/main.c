#include "stm32f10x.h"
#include "stm32f10x_it.h"
#include "./systick/bsp_SysTick.h"
#include "./led/bsp_led.h"
#include "./usart/bsp_usart.h"
#include "./i2c/bsp_i2c.h"
#include "./exti/bsp_exti.h"
#include "./dht11/bsp_dht11.h"
#include "./lcd/bsp_ili9341_lcd.h"
#include "mltypes.h"

#include "inv_mpu_dmp_motion_driver.h"
#include "log.h"
#include "invensense.h"
#include "invensense_adv.h"
#include "eMPL_outputs.h"
#include "mpu.h"
#include "inv_mpu.h"
#include "log.h"
#include "packet.h"


#define TASK_ENABLE 0
extern unsigned int Task_Delay[NumOfTask];

/* Private typedef -----------------------------------------------------------*/
/* Data read from MPL. */
#define PRINT_ACCEL     (0x01)
#define PRINT_GYRO      (0x02)
#define PRINT_QUAT      (0x04)
#define PRINT_COMPASS   (0x08)
#define PRINT_EULER     (0x10)
#define PRINT_ROT_MAT   (0x20)
#define PRINT_HEADING   (0x40)
#define PRINT_PEDO      (0x80)
#define PRINT_LINEAR_ACCEL (0x100)
#define PRINT_GRAVITY_VECTOR (0x200)

volatile uint32_t hal_timestamp = 0;
#define ACCEL_ON        (0x01)
#define GYRO_ON         (0x02)
#define COMPASS_ON      (0x04)

#define MOTION          (0)
#define NO_MOTION       (1)

/* Starting sampling rate. */
#define DEFAULT_MPU_HZ  (20)

#define FLASH_SIZE      (512)
#define FLASH_MEM_START ((void*)0x1800)

#define PEDO_READ_MS    (1000)
#define TEMP_READ_MS    (500)
#define COMPASS_READ_MS (100)

float Pitch,Roll,Yaw;
float accelx,accely,accelz;
float gyrox,gyroy,gyroz;
struct rx_s
{
  unsigned char header[3];
  unsigned char cmd;
};

struct hal_s
{
  unsigned char lp_accel_mode;
  unsigned char sensors;
  unsigned char dmp_on;
  unsigned char wait_for_tap;
  volatile unsigned char new_gyro;
  unsigned char motion_int_mode;
  unsigned long no_dmp_hz;
  unsigned long next_pedo_ms;
  unsigned long next_temp_ms;
  unsigned long next_compass_ms;
  unsigned int report;
  unsigned short dmp_features;
  struct rx_s rx;
};
static struct hal_s hal = {0};

/* USB RX binary semaphore. Actually, it's just a flag. Not included in struct
 * because it's declared extern elsewhere.
 */
volatile unsigned char rx_new;

unsigned char *mpl_key = (unsigned char*)"eMPL 5.1";

/* Platform-specific information. Kinda like a boardfile. */
struct platform_data_s
{
  signed char orientation[9];
};

/* The sensors can be mounted onto the board in any orientation. The mounting
 * matrix seen below tells the MPL how to rotate the raw data from the
 * driver(s).
 * TODO: The following matrices refer to the configuration on internal test
 * boards at Invensense. If needed, please modify the matrices to match the
 * chip-to-body matrix for your particular set up.
 */
static struct platform_data_s gyro_pdata =
{

  .orientation = {
    1, 0, 0,
    0, 1, 0,
    0, 0, 1
  }
};


/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* ---------------------------------------------------------------------------*/



#define BYTE0(dwTemp)       (*(char *)(&dwTemp))
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))
#define BYTE2(dwTemp)       (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((char *)(&dwTemp) + 3))


extern struct inv_sensor_cal_t sensors;


/* Get data from MPL.
 * TODO: Add return values to the inv_get_sensor_type_xxx APIs to differentiate
 * between new and stale data.
 */
static void read_from_mpl(void)
{
  long msg, data[9];
	
  int8_t accuracy;
  unsigned long timestamp;
  float float_data[3] = {0};

  MPU_DEBUG_FUNC();

  /*********发送数据到上位机**********/
  if(1)
    {

      unsigned long timestamp,step_count,walk_time;


      /*获取欧拉角*/
      if (inv_get_sensor_type_euler(data, &accuracy,(inv_time_t*)&timestamp))
        {
          
          Pitch =data[0]*1.0/(1<<16) ;
          Roll = data[1]*1.0/(1<<16);
          Yaw = data[2]*1.0/(1<<16);



        }

      if (inv_get_sensor_type_accel(data, &accuracy,(inv_time_t*)&timestamp))
        {
          
          accelx =data[0]*1.0/(1<<16) ;
          accely = data[1]*1.0/(1<<16);
          accelz = data[2]*1.0/(1<<16);
        }
      if (inv_get_sensor_type_gyro(data, &accuracy,(inv_time_t*)&timestamp))
        {
          
          gyrox =data[0]*1.0/(1<<16) ;
          gyroy = data[1]*1.0/(1<<16);
          gyroz = data[2]*1.0/(1<<16);
        }

      /*获取步数*/
      get_tick_count(&timestamp);
      if (timestamp > hal.next_pedo_ms)
        {

          hal.next_pedo_ms = timestamp + PEDO_READ_MS;
          dmp_get_pedometer_step_count(&step_count);
          dmp_get_pedometer_walk_time(&walk_time);

        }
    }
}


static inline void run_self_test(void)
{
  int result;
  long gyro[3], accel[3];
  MPU_DEBUG_FUNC();
#if defined (MPU6500) || defined (MPU9250)
  result = mpu_run_6500_self_test(gyro, accel, 0);
#elif defined (MPU6050) || defined (MPU9150)
  result = mpu_run_self_test(gyro, accel);
#endif
  if (result == 0x7)
    {
      MPL_LOGI("Passed!\n");
      MPL_LOGI("accel: %7.4f %7.4f %7.4f\n",
               accel[0]/65536.f,
               accel[1]/65536.f,
               accel[2]/65536.f);
      MPL_LOGI("gyro: %7.4f %7.4f %7.4f\n",
               gyro[0]/65536.f,
               gyro[1]/65536.f,
               gyro[2]/65536.f);
      /* Test passed. We can trust the gyro data here, so now we need to update calibrated data*/

#ifdef USE_CAL_HW_REGISTERS
      /*
       * This portion of the code uses the HW offset registers that are in the MPUxxxx devices
       * instead of pushing the cal data to the MPL software library
       */
      unsigned char i = 0;

      for(i = 0; i<3; i++)
        {
          gyro[i] = (long)(gyro[i] * 32.8f); //convert to +-1000dps
          accel[i] *= 2048.f; //convert to +-16G
          accel[i] = accel[i] >> 16;
          gyro[i] = (long)(gyro[i] >> 16);
        }

      mpu_set_gyro_bias_reg(gyro);

#if defined (MPU6500) || defined (MPU9250)
      mpu_set_accel_bias_6500_reg(accel);
#elif defined (MPU6050) || defined (MPU9150)
      mpu_set_accel_bias_6050_reg(accel);
#endif
#else
      /* Push the calibrated data to the MPL library.
       *
       * MPL expects biases in hardware units << 16, but self test returns
      * biases in g's << 16.
      */
      unsigned short accel_sens;
      float gyro_sens;

      mpu_get_accel_sens(&accel_sens);
      accel[0] *= accel_sens;
      accel[1] *= accel_sens;
      accel[2] *= accel_sens;

      inv_set_accel_bias(accel, 3);
      mpu_get_gyro_sens(&gyro_sens);
      gyro[0] = (long) (gyro[0] * gyro_sens);
      gyro[1] = (long) (gyro[1] * gyro_sens);
      gyro[2] = (long) (gyro[2] * gyro_sens);
      inv_set_gyro_bias(gyro, 3);
#endif
    }
  else
    {
      if (!(result & 0x1))
        MPL_LOGE("Gyro failed.\n");
      if (!(result & 0x2))
        MPL_LOGE("Accel failed.\n");
      if (!(result & 0x4))
        MPL_LOGE("Compass failed.\n");
    }

}


void gyro_data_ready_cb(void)
{

  hal.new_gyro = 1;
}

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{

  inv_error_t result;
  unsigned char accel_fsr,  new_temp = 0;
  unsigned short gyro_rate, gyro_fsr;
  unsigned long timestamp;
  struct int_param_s int_param;
	  /* 串口通信初始化 */
	USART_Config();
	printf("usart init...\n");
	
	ILI9341_Init();
	ILI9341_GramScan(6);
	LCD_SetColors(WHITE, BLACK);
	ILI9341_Clear(0,0,240,320);
	
  SysTick_Init();
  SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;
	printf("systick init...\n");
  /* LED 端口初始化 */
  LED_GPIO_Config();
  LED_BLUE;


  
  //MPU6050中断引脚
  EXTI_Pxy_Config();
	printf("mpu init...\n");
  //I2C初始化
  I2C_Bus_Init();
	printf("i2c init...\n");
	DHT11_Init();
	DHT11_TypeDef DHT11TypeStructure;
  printf("mpu 6050 test start");


  result = mpu_init(&int_param);
  if (result)
    {
      LED_RED;
      MPL_LOGE("Could not initialize gyro.result =  %d\n",result);

    }
  else
    {
      LED_GREEN;

    }

  /* If you're not using an MPU9150 AND you're not using DMP features, this
    * function will place all slaves on the primary bus.
    * mpu_set_bypass(1);
    */

  result = inv_init_mpl();
  if (result)
    {
      MPL_LOGE("Could not initialize MPL.\n");
    }

  /* Compute 6-axis and 9-axis quaternions. */
  inv_enable_quaternion();
  inv_enable_9x_sensor_fusion();
  /* The MPL expects compass data at a constant rate (matching the rate
   * passed to inv_set_compass_sample_rate). If this is an issue for your
   * application, call this function, and the MPL will depend on the
   * timestamps passed to inv_build_compass instead.
   *
   * inv_9x_fusion_use_timestamps(1);
   */

  /* This function has been deprecated.
   * inv_enable_no_gyro_fusion();
   */

  /* Update gyro biases when not in motion.
   * WARNING: These algorithms are mutually exclusive.
   */
  inv_enable_fast_nomot();
  /* inv_enable_motion_no_motion(); */
  /* inv_set_no_motion_time(1000); */

  /* Update gyro biases when temperature changes. */
  inv_enable_gyro_tc();

  /* If you need to estimate your heading before the compass is calibrated,
   * enable this algorithm. It becomes useless after a good figure-eight is
   * detected, so we'll just leave it out to save memory.
   * inv_enable_heading_from_gyro();
   */

  /* Allows use of the MPL APIs in read_from_mpl. */
  inv_enable_eMPL_outputs();

  result = inv_start_mpl();

  if (result == INV_ERROR_NOT_AUTHORIZED)
    {
      while (1)
        {
          MPL_LOGE("Not authorized.\n");
        }
    }
  if (result)
    {
      MPL_LOGE("Could not start the MPL.\n");
    }

  /* Get/set hardware configuration. Start gyro. */
  /* Wake up all sensors. */
#ifdef COMPASS_ENABLED
  mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL | INV_XYZ_COMPASS);
#else
  mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL);
#endif
  /* Push both gyro and accel data into the FIFO. */
  mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL);
  mpu_set_sample_rate(DEFAULT_MPU_HZ);

  /* Read back configuration in case it was set improperly. */
  mpu_get_sample_rate(&gyro_rate);
  mpu_get_gyro_fsr(&gyro_fsr);
  mpu_get_accel_fsr(&accel_fsr);

  /* Sync driver configuration with MPL. */
  /* Sample rate expected in microseconds. */
  inv_set_gyro_sample_rate(1000000L / gyro_rate);
  inv_set_accel_sample_rate(1000000L / gyro_rate);

  /* Set chip-to-body orientation matrix.
   * Set hardware units to dps/g's/degrees scaling factor.
   */
  inv_set_gyro_orientation_and_scale(
    inv_orientation_matrix_to_scalar(gyro_pdata.orientation),
    (long)gyro_fsr<<15);
  inv_set_accel_orientation_and_scale(
    inv_orientation_matrix_to_scalar(gyro_pdata.orientation),
    (long)accel_fsr<<15);

  /* Initialize HAL state variables. */
#ifdef COMPASS_ENABLED
  hal.sensors = ACCEL_ON | GYRO_ON | COMPASS_ON;
#else
  hal.sensors = ACCEL_ON | GYRO_ON;
#endif
  hal.dmp_on = 0;
  hal.report = 0;
  hal.rx.cmd = 0;
  hal.next_pedo_ms = 0;
  hal.next_compass_ms = 0;
  hal.next_temp_ms = 0;

  /* Compass reads are handled by scheduler. */
  get_tick_count(&timestamp);

  /* To initialize the DMP:
   * 1. Call dmp_load_motion_driver_firmware(). This pushes the DMP image in
   *    inv_mpu_dmp_motion_driver.h into the MPU memory.
   * 2. Push the gyro and accel orientation matrix to the DMP.
   * 3. Register gesture callbacks. Don't worry, these callbacks won't be
   *    executed unless the corresponding feature is enabled.
   * 4. Call dmp_enable_feature(mask) to enable different features.
   * 5. Call dmp_set_fifo_rate(freq) to select a DMP output rate.
   * 6. Call any feature-specific control functions.
   *
   * To enable the DMP, just call mpu_set_dmp_state(1). This function can
   * be called repeatedly to enable and disable the DMP at runtime.
   *
   * The following is a short summary of the features supported in the DMP
   * image provided in inv_mpu_dmp_motion_driver.c:
   * DMP_FEATURE_LP_QUAT: Generate a gyro-only quaternion on the DMP at
   * 200Hz. Integrating the gyro data at higher rates reduces numerical
   * errors (compared to integration on the MCU at a lower sampling rate).
   * DMP_FEATURE_6X_LP_QUAT: Generate a gyro/accel quaternion on the DMP at
   * 200Hz. Cannot be used in combination with DMP_FEATURE_LP_QUAT.
   * DMP_FEATURE_TAP: Detect taps along the X, Y, and Z axes.
   * DMP_FEATURE_ANDROID_ORIENT: Google's screen rotation algorithm. Triggers
   * an event at the four orientations where the screen should rotate.
   * DMP_FEATURE_GYRO_CAL: Calibrates the gyro data after eight seconds of
   * no motion.
   * DMP_FEATURE_SEND_RAW_ACCEL: Add raw accelerometer data to the FIFO.
   * DMP_FEATURE_SEND_RAW_GYRO: Add raw gyro data to the FIFO.
   * DMP_FEATURE_SEND_CAL_GYRO: Add calibrated gyro data to the FIFO. Cannot
   * be used in combination with DMP_FEATURE_SEND_RAW_GYRO.
   */

  dmp_load_motion_driver_firmware();
  dmp_set_orientation(
    inv_orientation_matrix_to_scalar(gyro_pdata.orientation));
  /*
   * Known Bug -
   * DMP when enabled will sample sensor data at 200Hz and output to FIFO at the rate
   * specified in the dmp_set_fifo_rate API. The DMP will then sent an interrupt once
   * a sample has been put into the FIFO. Therefore if the dmp_set_fifo_rate is at 25Hz
   * there will be a 25Hz interrupt from the MPU device.
   *
   * There is a known issue in which if you do not enable DMP_FEATURE_TAP
   * then the interrupts will be at 200Hz even if fifo rate
   * is set at a different rate. To avoid this issue include the DMP_FEATURE_TAP
   *
   * DMP sensor fusion works only with gyro at +-2000dps and accel +-2G
   */
  hal.dmp_features = DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_TAP |
                     DMP_FEATURE_ANDROID_ORIENT | DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_SEND_CAL_GYRO |
                     DMP_FEATURE_GYRO_CAL;
  dmp_enable_feature(hal.dmp_features);
  dmp_set_fifo_rate(DEFAULT_MPU_HZ);
  mpu_set_dmp_state(1);
  hal.dmp_on = 1;


  while(1)
    {

      unsigned long sensor_timestamp;
      int new_data = 0;
      if (USART_GetFlagStatus (DEBUG_USARTx, USART_FLAG_RXNE))
        {
          /* A byte has been received via USART. See handle_input for a list of
           * valid commands.
           */

          USART_ClearFlag(DEBUG_USARTx, USART_FLAG_RXNE);

          //handle_input();


        }
      get_tick_count(&timestamp);

      /* Temperature data doesn't need to be read with every gyro sample.
       * Let's make them timer-based like the compass reads.
       */
      if (timestamp > hal.next_temp_ms)
        {
          hal.next_temp_ms = timestamp + TEMP_READ_MS;
          new_temp = 1;
        }

      if (hal.motion_int_mode)
        {
          /* Enable motion interrupt. */
          mpu_lp_motion_interrupt(500, 1, 5);
          /* Notify the MPL that contiguity was broken. */
          inv_accel_was_turned_off();
          inv_gyro_was_turned_off();
          inv_compass_was_turned_off();
          inv_quaternion_sensor_was_turned_off();
          /* Wait for the MPU interrupt. */
          while (!hal.new_gyro) {}
          /* Restore the previous sensor configuration. */
          mpu_lp_motion_interrupt(0, 0, 0);
          hal.motion_int_mode = 0;
        }

      if (!hal.sensors || !hal.new_gyro)
        {
          continue;
        }
      if (hal.new_gyro && hal.lp_accel_mode)
        {
          short accel_short[3];
          long accel[3];
          mpu_get_accel_reg(accel_short, &sensor_timestamp);
          accel[0] = (long)accel_short[0];
          accel[1] = (long)accel_short[1];
          accel[2] = (long)accel_short[2];
          inv_build_accel(accel, 0, sensor_timestamp);
          new_data = 1;
          hal.new_gyro = 0;
        }
      else if (hal.new_gyro && hal.dmp_on)
        {
          short gyro[3], accel_short[3], sensors;
          unsigned char more;
          long accel[3], quat[4], temperature;
          /* This function gets new data from the FIFO when the DMP is in
           * use. The FIFO can contain any combination of gyro, accel,
           * quaternion, and gesture data. The sensors parameter tells the
           * caller which data fields were actually populated with new data.
           * For example, if sensors == (INV_XYZ_GYRO | INV_WXYZ_QUAT), then
           * the FIFO isn't being filled with accel data.
           * The driver parses the gesture data to determine if a gesture
           * event has occurred; on an event, the application will be notified
           * via a callback (assuming that a callback function was properly
           * registered). The more parameter is non-zero if there are
           * leftover packets in the FIFO.
           */
          dmp_read_fifo(gyro, accel_short, quat, &sensor_timestamp, &sensors, &more);
          if (!more)
            hal.new_gyro = 0;
          if (sensors & INV_XYZ_GYRO)
            {
              /* Push the new data to the MPL. */
              inv_build_gyro(gyro, sensor_timestamp);
              new_data = 1;
              if (new_temp)
                {
                  new_temp = 0;
                  /* Temperature only used for gyro temp comp. */
                  mpu_get_temperature(&temperature, &sensor_timestamp);
                  inv_build_temp(temperature, sensor_timestamp);
                }
            }
          if (sensors & INV_XYZ_ACCEL)
            {
              accel[0] = (long)accel_short[0];
              accel[1] = (long)accel_short[1];
              accel[2] = (long)accel_short[2];
              inv_build_accel(accel, 0, sensor_timestamp);
              new_data = 1;
            }
          if (sensors & INV_WXYZ_QUAT)
            {
              inv_build_quat(quat, 0, sensor_timestamp);
              new_data = 1;
            }
        }
      else if (hal.new_gyro)
        {
          short gyro[3], accel_short[3];
          unsigned char sensors, more;
          long accel[3], temperature;
          /* This function gets new data from the FIFO. The FIFO can contain
           * gyro, accel, both, or neither. The sensors parameter tells the
           * caller which data fields were actually populated with new data.
           * For example, if sensors == INV_XYZ_GYRO, then the FIFO isn't
           * being filled with accel data. The more parameter is non-zero if
           * there are leftover packets in the FIFO. The HAL can use this
           * information to increase the frequency at which this function is
           * called.
           */
          hal.new_gyro = 0;
          mpu_read_fifo(gyro, accel_short, &sensor_timestamp,
                        &sensors, &more);
          if (more)
            hal.new_gyro = 1;
          if (sensors & INV_XYZ_GYRO)
            {
              /* Push the new data to the MPL. */
              inv_build_gyro(gyro, sensor_timestamp);
              new_data = 1;
              if (new_temp)
                {
                  new_temp = 0;
                  /* Temperature only used for gyro temp comp. */
                  mpu_get_temperature(&temperature, &sensor_timestamp);
                  inv_build_temp(temperature, sensor_timestamp);
                }
            }
          if (sensors & INV_XYZ_ACCEL)
            {
              accel[0] = (long)accel_short[0];
              accel[1] = (long)accel_short[1];
              accel[2] = (long)accel_short[2];
              inv_build_accel(accel, 0, sensor_timestamp);
              new_data = 1;
            }
        }


      if (new_data)
        {
          inv_execute_on_data();

          /* This function reads bias-compensated sensor data and sensor
           * fusion outputs from the MPL. The outputs are formatted as seen
           * in eMPL_outputs.c. This function only needs to be called at the
           * rate requested by the host.
           */
          read_from_mpl();
          
					//mdelay(1000);
        }
				
				if(!DHT11_Read_TempAndHumi_ToStruct(&DHT11TypeStructure)){

					
				}
				
				char str[256];
				printf("start\n");
				//ILI9341_DispString_EN(20,0,str);
				//欧拉角三组数据
				sprintf(str, "Pitch:%.4f", Pitch);
				printf("%s\n",str);
				ILI9341_DispString_EN(20,20,str);
				sprintf(str, "Roll:%.4f", Roll);
				printf("%s\n",str);
				ILI9341_DispString_EN(20,35,str);
				sprintf(str, "Yaw:%.4f", Yaw);
				printf("%s\n",str);
				ILI9341_DispString_EN(20,50,str);
				//陀螺仪三组数据
				sprintf(str, "accelx:%.4f", accelx);
				printf("%s\n",str);
				ILI9341_DispString_EN(20,65,str);
				sprintf(str, "accely:%.4f", accely);
				printf("%s\n",str);
				ILI9341_DispString_EN(20,80,str);
				sprintf(str, "accelz:%.4f", accelz);
				printf("%s\n",str);
				ILI9341_DispString_EN(20,95,str);
				//加速度三组数据
				sprintf(str, "gyrox:%.4f", gyrox);
				printf("%s\n",str);
				ILI9341_DispString_EN(20,110,str);
				sprintf(str, "gyroy:%.4f", gyroy);
				printf("%s\n",str);
				ILI9341_DispString_EN(20,125,str);
				sprintf(str, "gyroz:%.4f", gyroz);
				printf("%s\n",str);
				ILI9341_DispString_EN(20,140,str);
				//温湿度数据
				sprintf(str, "hum:%d.%d", DHT11TypeStructure.humi_int,DHT11TypeStructure.humi_dect);
				printf("%s\n",str);
				ILI9341_DispString_EN(20,155,str);
				sprintf(str, "temp:%d.%d", DHT11TypeStructure.temp_int,DHT11TypeStructure.temp_dect);
				printf("%s\n",str);
				ILI9341_DispString_EN(20,170,str);
				printf("over\n");
				mdelay(1000);
    }





}

