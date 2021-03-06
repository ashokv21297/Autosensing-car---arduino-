

#include <helper_3dmath.h>
#include <MPU6050.h>
#include <MPU6050_6Axis_MotionApps20.h>
#include <MPU6050_9Axis_MotionApps41.h>

#include<I2Cdev.h>
#include<Wire.h>
int buffersize=100;
int acel_deadzone=8;
int giro_deadzone=1;
MPU6050 accelgyro(0x68);
int16_t ax,ay,az,gx,gy,gz;
int mean_ax,mean_ay,mean_az,mean_gx,mean_gy,mean_gz,state=0;
int ax_offset,ay_offset,az_offset,gx_offset,gy_offset,gz_offset;
void meansensors();
void calibration(); 
void setup()
{
  Wire.begin();
  TWBR=24;
  Serial.begin(115200);
  accelgyro.initialize();
  while(Serial.available()&&Serial.read());
  while(!Serial.available())
  {
    Serial.println(F("Send any character to start sketch:\n" ));
    delay(1500);
  }
  while(Serial.available()&&Serial.read());
  Serial.println("\n MPU6050 calibration");
  delay(2000);
  Serial.println(accelgyro.testConnection()?"MPU6050 connection successful"  :"MPU6050 connection failed");
  delay(1000);
  accelgyro.setXAccelOffset(0);
  accelgyro.setYAccelOffset(0);
  accelgyro.setZAccelOffset(0);
  accelgyro.setXGyroOffset(0);
  accelgyro.setYGyroOffset(0);
  accelgyro.setZGyroOffset(0);
}

void loop()
{
  if(state==0)
  {
    Serial.println("\n Reading sensor for first time");
    meansensors();
    state++;
    delay(1000);
  }
  if(state==1)
  {
    Serial.println("\n Calculating offsets");
    calibration();
    state++;
    delay(1000);
  }
  if(state==2)
  {
    meansensor();
    Serial.println("\n Finished");
    Serial.print("Sensor reading with offsets:\t");
    Serial.print(mean_ax);
    Serial.print("\t");
    Serial.print(mean_ay);
    Serial.print("\t");
    Serial.print(mean_az);
    Serial.print("\t");
    Serial.print(mean_gx);
    Serial.print("\t");
    Serial.print(mean_gy);
    Serial.print("\t");
    Serial.println(mean_gz);
    Serial.print("\n");
    Serial.print("Your offsets:\t");
    Serial.print(ax_offset);
    Serial.print("\t");
    Serial.print(ay_offset);
    Serial.print("\t");
    Serial.print(az_offset);
    Serial.print("\t");
    Serial.print(gx_offset);
    Serial.print("\t");
    Serial.print(gy_offset);
    Serial.print("\t");
    Serial.println(gz_offset);
    Serial.print("\t");
    while(1);
    
  }
}

void meansensor()
{
  long i=0,buff_ax=0,buff_ay=0,buff_az=0,buff_gx=0,buff_gy=0,buff_gz=0;
  while(i<(buffersize+101))
  {
    accelgyro.getMotion(&ax,&ay,&az,&gx,&gy,&gz);
    if(i>100&&i<=(buffersize+100))
    {
      buff_ax=buff_ax+ax;
      buff_ay=buff_ay+ay;
      buff_az=buff_az+az;
      buff_gx=buff_gx+gx;
      buff_gy=buff_gy+gy;
      buff_gz=buff_gz+gz;
    }
    if(i==(buffersize+100))
    {
      mean_ax=buff_ax/buffersize;
      mean_ay=buff_ay/buffersize;
      mean_az=buff_az/buffersize;
      mean_gx=buff_gx/buffersize;
      mean_gy=buff_gy/buffersize;
      mean_gz=buff_gz/buffersize;
    }
    i++;
    delay(2);
  }
 }
  void calibration()
  {
    ax_offset=-mean_ax/8;
    ay_offset=-mean_ay/8;
    ax_offset=(16384.0-mean_az)/8;
    gx_offset=-mean_gx/4;
    gy_offset=-mean_gy/4;
    gz_offset=-mean_gz/4;
    while(1)
    {
      int ready=0;
      accelgyro.setXAccelOffset(ax_offset);
      accelgyro.setYAccelOffset(ay_offset);
      accelgyro.setZAccelOffset(az_offset);
      accelgyro.setXGyroOffset(gx_offset);
      accelgyro.setYGyroOffset(gy_offset);
      accelgyro.setZGyroOffset(gz_offset);
      meansensors();
      Serial.println(".......");
      if(abs(mean_ax)<=acel_deadzone)ready++;
      else ax_offset=ax_offset-mean_ax/acel_deadzone;
      if(abs(mean_ay)<=acel_deadzone)ready++;
      else ay_offset=ay_offset-mean_ay/acel_deadzone;
      if(abs(16384.0-mean_az)<=acel_deadzone)ready++;
      else az_offset=az_offset+(16384.0-mean_az)/acel_deadzone;
      if(abs(mean_gx)<=giro_deadzone)ready++;
      else gx_offset=gx_offset-mean_gx/(gyro_deadzone+1);
      if(abs(mean_gy)<=giro_deadzone)ready++;
      else gy_offset=gy_offset-mean_gy/(gyro_deadzone+1);
      if(abs(mean_gz)<=giro_deadzone)ready++;
      else gz_offset=gz_offset-mean_gz/(gyro_deadzone+1);
      if(ready==6)break;
    }
    
    }

