#include "mpu6050.h"

static void I2C_Init(void); // I2C 初始化 
static void I2C_Start(void); // 产生 I2C 协议起始信号 
static void I2C_Stop(void); // 产生 I2C 协议结束信号 
static void I2C_Write_Byte(uint8_t byte); // 发送八位数据（不包含应答） 
static uint8_t I2C_Read_Byte(void); // 读取八位数据（不包含应答） 
static uint8_t I2C_Read_ACK(void); // 接收应答信号 
static void I2C_Write_ACK(uint8_t ack); // 发送应答信号

static void I2C_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
		
	GPIO_Initure.Pin=GPIO_PIN_15;//PA15-----SCL
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;
	GPIO_Initure.Pull=GPIO_PULLUP;
	GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);
	
	GPIO_Initure.Pin=GPIO_PIN_3|GPIO_PIN_1;//PB3-----SDA
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;
	GPIO_Initure.Pull=GPIO_PULLUP;
	GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOB,&GPIO_Initure);
	
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_SET);
	delay_ms(100);
}

static void I2C_Start(void)
{
	IIC_SDA(1);
	IIC_SCL(1);
	delay_us(2);
	IIC_SDA(0);
	delay_us(2);
	IIC_SCL(0);
	delay_us(2);
}

static void I2C_Stop(void)
{
	IIC_SDA(0);
	IIC_SCL(1);
	delay_us(2);
	IIC_SDA(1);
	delay_us(2);
}

static void I2C_Write_Byte(uint8_t byte)
{
	uint16_t i;
	
	IIC_SCL(0);  // 改变数据线前拉低时钟
	
	for (i = 0; i < 8; i++)
	{
		if (byte & (0x80 >> i))
		{
			IIC_SDA(1);
		}
		else
		{
			IIC_SDA(0);
		}
		delay_us(2);		// 必须加延时，等待数据准备好
		
		IIC_SCL(1);
		delay_us(2);
		IIC_SCL(0);
		delay_us(2);
	}
}

static uint8_t I2C_Read_Byte(void)
{
	uint8_t byte;
	uint16_t i;
	
	for (i = 0; i < 8; i++)
	{
		IIC_SDA(1);  // 拉高数据线，读取数据
		IIC_SCL(1);
		delay_us(2);
		byte <<= 1;
		byte |= SDA;
		IIC_SCL(0);
		delay_us(2);
	}
	
	return byte;
}

static uint8_t I2C_Read_ACK(void)
{
	uint8_t ack;
	
	IIC_SDA(1);  // 拉高数据线，读取数据
	IIC_SCL(1);
	delay_us(2);
	ack = SDA;
	IIC_SCL(0);
	delay_us(2);
	
	return ack;
}

static void I2C_Write_ACK(uint8_t ack)
{
	IIC_SCL(0);  // 改变数据线前拉低时钟
	
	if (ack)
	{
		IIC_SDA(1);
	}
	else
	{
		IIC_SDA(0);
	}
	
	IIC_SCL(1);
	delay_us(2);
	IIC_SCL(0);
	delay_us(2);
}

//---------------------------------------------------------------------------------------------------------------------------------------


/******************************************************************************
* 函数介绍： MPU6050 初始化函数
* 输入参数： 无
* 输出参数： 无
* 返回值 ：  无
* 备   注：  配置 MPU6050 测量范围：± 2000 °/s  ± 2g
******************************************************************************/
void MPU6050_Init(void)
{
	I2C_Init();  // I2C 初始化
	
	MPU6050_Write_Reg(PWR_MGMT_1, 0x00);	//解除休眠状态
	MPU6050_Write_Reg(SMPLRT_DIV, 0x07);    //陀螺仪采样率，典型值：0x07(125Hz)
	MPU6050_Write_Reg(CONFIG, 0x06);        //低通滤波频率，典型值：0x06(5Hz)
	MPU6050_Write_Reg(GYRO_CONFIG, 0x18);   //陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
	MPU6050_Write_Reg(ACCEL_CONFIG, 0x01);  //加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz)
}

/******************************************************************************
* 函数介绍： MPU6050 写寄存器函数
* 输入参数： regAddr：寄存器地址
             regData：待写入寄存器值
* 输出参数： 无
* 返回值 ：  无
******************************************************************************/
void MPU6050_Write_Reg(uint8_t regAddr, uint8_t regData)
{
	/* 发送起始信号 */
	I2C_Start();
	
	/* 发送设备地址 */		
	I2C_Write_Byte(DEV_ADDR);
	if (I2C_Read_ACK())
		goto stop;
	
	/* 发送寄存器地址 */
	I2C_Write_Byte(regAddr);
	if (I2C_Read_ACK())
		goto stop;
	
	/* 写数据到寄存器 */
	I2C_Write_Byte(regData);
	if (I2C_Read_ACK())
		goto stop;
	
stop:
	I2C_Stop();
}

/******************************************************************************
* 函数介绍： MPU6050 读寄存器函数
* 输入参数： regAddr：寄存器地址
* 输出参数： 无
* 返回值 ：  regData：读出的寄存器数据
******************************************************************************/
uint8_t MPU6050_Read_Reg(uint8_t regAddr)
{
	uint8_t regData;
	
	/* 发送起始信号 */
	I2C_Start();
	
		/* 发送设备地址 */		
	I2C_Write_Byte(DEV_ADDR);
	if (I2C_Read_ACK())
		goto stop;
	
	/* 发送寄存器地址 */
	I2C_Write_Byte(regAddr);
	if (I2C_Read_ACK())
		goto stop;
	
	/* 发送重复起始信号 */
	I2C_Start();
	
	/* 发送读模式设备地址 */		
	I2C_Write_Byte(DEV_ADDR | 0x01);
	if (I2C_Read_ACK())
		goto stop;
	
	/* 读寄存器数据 */
	regData = I2C_Read_Byte();
	I2C_Write_ACK(1);  // 非应答信号
	
stop:
	I2C_Stop();
	
	return regData;
}

/******************************************************************************
* 函数介绍： 连续读两个寄存器并合成 16 位数据
* 输入参数： regAddr：数据低位寄存器地址
* 输出参数： 无
* 返回值 ：  data：2 个寄存器合成的 16 位数据
******************************************************************************/
int16_t MPU6050_Get_Data(uint8_t regAddr)
{
	uint8_t Data_H, Data_L;
	uint16_t data;
	
	Data_H = MPU6050_Read_Reg(regAddr);
	Data_L = MPU6050_Read_Reg(regAddr + 1);
	data = (Data_H << 8) | Data_L;  // 合成数据
	
	return data;
}

/****************************************************************************** 
* 函数介绍： 串口打印 6050 传感器读取的三轴加速度、陀螺仪及温度数据 
* 输入参数： 无 
* 输出参数： 无 
* 返回值 ： 无 
******************************************************************************/
void MPU6050_Display(void)
{
    /* 打印 x, y, z 轴加速度 */
    printf("ACCEL_X: %d\t", MPU6050_Get_Data(ACCEL_XOUT_H));
    printf("ACCEL_Y: %d\t", MPU6050_Get_Data(ACCEL_YOUT_H));
    printf("ACCEL_Z: %d\t", MPU6050_Get_Data(ACCEL_ZOUT_H));
    
    /* 打印温度，需要根据手册的公式换算为摄氏度 */
    printf("TEMP: %0.2f\t", MPU6050_Get_Data(TEMP_OUT_H) / 340.0 + 36.53);
    
    /* 打印 x, y, z 轴角速度 */
    printf("GYRO_X: %d\t", MPU6050_Get_Data(GYRO_XOUT_H));
    printf("GYRO_Y: %d\t", MPU6050_Get_Data(GYRO_YOUT_H));
    printf("GYRO_Z: %d\t", MPU6050_Get_Data(GYRO_ZOUT_H));
    
    printf("\r\n");
}
/****************************************************************************** 
* 函数介绍： 计算 x, y, z 三轴的倾角 
* 输入参数： 无 
* 输出参数： data：角度结构体 
* 返回值 ： 无 
******************************************************************************/
void MPU6050_Get_Angle(MPU6050_Angle *data)
{   
    /* 计算x, y, z 轴倾角，返回弧度值*/
    data->X_Angle = acos((MPU6050_Get_Data(ACCEL_XOUT_H) + X_ACCEL_OFFSET) / 16384.0);
    data->Y_Angle = acos((MPU6050_Get_Data(ACCEL_YOUT_H) + Y_ACCEL_OFFSET) / 16384.0);
    data->Z_Angle = acos((MPU6050_Get_Data(ACCEL_ZOUT_H) + Z_ACCEL_OFFSET) / 16384.0);

    /* 弧度值转换为角度值 */
    data->X_Angle = data->X_Angle * 57.29577;
    data->Y_Angle = data->Y_Angle * 57.29577;
    data->Z_Angle = data->Z_Angle * 57.29577;
} 