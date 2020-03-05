#include "mpu6050.h"

static void I2C_Init(void); // I2C ��ʼ�� 
static void I2C_Start(void); // ���� I2C Э����ʼ�ź� 
static void I2C_Stop(void); // ���� I2C Э������ź� 
static void I2C_Write_Byte(uint8_t byte); // ���Ͱ�λ���ݣ�������Ӧ�� 
static uint8_t I2C_Read_Byte(void); // ��ȡ��λ���ݣ�������Ӧ�� 
static uint8_t I2C_Read_ACK(void); // ����Ӧ���ź� 
static void I2C_Write_ACK(uint8_t ack); // ����Ӧ���ź�

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
	
	IIC_SCL(0);  // �ı�������ǰ����ʱ��
	
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
		delay_us(2);		// �������ʱ���ȴ�����׼����
		
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
		IIC_SDA(1);  // ���������ߣ���ȡ����
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
	
	IIC_SDA(1);  // ���������ߣ���ȡ����
	IIC_SCL(1);
	delay_us(2);
	ack = SDA;
	IIC_SCL(0);
	delay_us(2);
	
	return ack;
}

static void I2C_Write_ACK(uint8_t ack)
{
	IIC_SCL(0);  // �ı�������ǰ����ʱ��
	
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
* �������ܣ� MPU6050 ��ʼ������
* ��������� ��
* ��������� ��
* ����ֵ ��  ��
* ��   ע��  ���� MPU6050 ������Χ���� 2000 ��/s  �� 2g
******************************************************************************/
void MPU6050_Init(void)
{
	I2C_Init();  // I2C ��ʼ��
	
	MPU6050_Write_Reg(PWR_MGMT_1, 0x00);	//�������״̬
	MPU6050_Write_Reg(SMPLRT_DIV, 0x07);    //�����ǲ����ʣ�����ֵ��0x07(125Hz)
	MPU6050_Write_Reg(CONFIG, 0x06);        //��ͨ�˲�Ƶ�ʣ�����ֵ��0x06(5Hz)
	MPU6050_Write_Reg(GYRO_CONFIG, 0x18);   //�������Լ켰������Χ������ֵ��0x18(���Լ죬2000deg/s)
	MPU6050_Write_Reg(ACCEL_CONFIG, 0x01);  //���ټ��Լ졢������Χ����ͨ�˲�Ƶ�ʣ�����ֵ��0x01(���Լ죬2G��5Hz)
}

/******************************************************************************
* �������ܣ� MPU6050 д�Ĵ�������
* ��������� regAddr���Ĵ�����ַ
             regData����д��Ĵ���ֵ
* ��������� ��
* ����ֵ ��  ��
******************************************************************************/
void MPU6050_Write_Reg(uint8_t regAddr, uint8_t regData)
{
	/* ������ʼ�ź� */
	I2C_Start();
	
	/* �����豸��ַ */		
	I2C_Write_Byte(DEV_ADDR);
	if (I2C_Read_ACK())
		goto stop;
	
	/* ���ͼĴ�����ַ */
	I2C_Write_Byte(regAddr);
	if (I2C_Read_ACK())
		goto stop;
	
	/* д���ݵ��Ĵ��� */
	I2C_Write_Byte(regData);
	if (I2C_Read_ACK())
		goto stop;
	
stop:
	I2C_Stop();
}

/******************************************************************************
* �������ܣ� MPU6050 ���Ĵ�������
* ��������� regAddr���Ĵ�����ַ
* ��������� ��
* ����ֵ ��  regData�������ļĴ�������
******************************************************************************/
uint8_t MPU6050_Read_Reg(uint8_t regAddr)
{
	uint8_t regData;
	
	/* ������ʼ�ź� */
	I2C_Start();
	
		/* �����豸��ַ */		
	I2C_Write_Byte(DEV_ADDR);
	if (I2C_Read_ACK())
		goto stop;
	
	/* ���ͼĴ�����ַ */
	I2C_Write_Byte(regAddr);
	if (I2C_Read_ACK())
		goto stop;
	
	/* �����ظ���ʼ�ź� */
	I2C_Start();
	
	/* ���Ͷ�ģʽ�豸��ַ */		
	I2C_Write_Byte(DEV_ADDR | 0x01);
	if (I2C_Read_ACK())
		goto stop;
	
	/* ���Ĵ������� */
	regData = I2C_Read_Byte();
	I2C_Write_ACK(1);  // ��Ӧ���ź�
	
stop:
	I2C_Stop();
	
	return regData;
}

/******************************************************************************
* �������ܣ� �����������Ĵ������ϳ� 16 λ����
* ��������� regAddr�����ݵ�λ�Ĵ�����ַ
* ��������� ��
* ����ֵ ��  data��2 ���Ĵ����ϳɵ� 16 λ����
******************************************************************************/
int16_t MPU6050_Get_Data(uint8_t regAddr)
{
	uint8_t Data_H, Data_L;
	uint16_t data;
	
	Data_H = MPU6050_Read_Reg(regAddr);
	Data_L = MPU6050_Read_Reg(regAddr + 1);
	data = (Data_H << 8) | Data_L;  // �ϳ�����
	
	return data;
}

/****************************************************************************** 
* �������ܣ� ���ڴ�ӡ 6050 ��������ȡ��������ٶȡ������Ǽ��¶����� 
* ��������� �� 
* ��������� �� 
* ����ֵ �� �� 
******************************************************************************/
void MPU6050_Display(void)
{
    /* ��ӡ x, y, z ����ٶ� */
    printf("ACCEL_X: %d\t", MPU6050_Get_Data(ACCEL_XOUT_H));
    printf("ACCEL_Y: %d\t", MPU6050_Get_Data(ACCEL_YOUT_H));
    printf("ACCEL_Z: %d\t", MPU6050_Get_Data(ACCEL_ZOUT_H));
    
    /* ��ӡ�¶ȣ���Ҫ�����ֲ�Ĺ�ʽ����Ϊ���϶� */
    printf("TEMP: %0.2f\t", MPU6050_Get_Data(TEMP_OUT_H) / 340.0 + 36.53);
    
    /* ��ӡ x, y, z ����ٶ� */
    printf("GYRO_X: %d\t", MPU6050_Get_Data(GYRO_XOUT_H));
    printf("GYRO_Y: %d\t", MPU6050_Get_Data(GYRO_YOUT_H));
    printf("GYRO_Z: %d\t", MPU6050_Get_Data(GYRO_ZOUT_H));
    
    printf("\r\n");
}
/****************************************************************************** 
* �������ܣ� ���� x, y, z �������� 
* ��������� �� 
* ��������� data���ǶȽṹ�� 
* ����ֵ �� �� 
******************************************************************************/
void MPU6050_Get_Angle(MPU6050_Angle *data)
{   
    /* ����x, y, z ����ǣ����ػ���ֵ*/
    data->X_Angle = acos((MPU6050_Get_Data(ACCEL_XOUT_H) + X_ACCEL_OFFSET) / 16384.0);
    data->Y_Angle = acos((MPU6050_Get_Data(ACCEL_YOUT_H) + Y_ACCEL_OFFSET) / 16384.0);
    data->Z_Angle = acos((MPU6050_Get_Data(ACCEL_ZOUT_H) + Z_ACCEL_OFFSET) / 16384.0);

    /* ����ֵת��Ϊ�Ƕ�ֵ */
    data->X_Angle = data->X_Angle * 57.29577;
    data->Y_Angle = data->Y_Angle * 57.29577;
    data->Z_Angle = data->Z_Angle * 57.29577;
} 