#include <DHT.h>
#define DHTPIN A0
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#include <Servo.h>
Servo myServo;



//버튼 부분.

int state_down = 0; //상태를 나타내는 수

int state_up = 0;

int c =0;


boolean status = false;


//lcd부분

#include<Wire.h>
#include<LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);

float o = 25; //초기 온도값- 전역변수로 맞춰줌. //실제 온도는 측정온도보단 온도계로 재봐야 함.


//시간 관리부
unsigned long timeVal=0; //이전 시간
unsigned long millisTime = millis()/1000;
int d1 ;
int d2 ;
int d3 ;




void setup() {
  Serial.begin(9600);//시리얼 프린트 시작
  pinMode(8,OUTPUT); //릴레이 제어를 위한 핀
  pinMode(5,OUTPUT); //쿨러 회전을 위한 핀
  pinMode(6,OUTPUT); //쿨러 회전을 위한 핀
  pinMode(A0,INPUT); //온도 센서를 연결하는 핀

  //버튼
  pinMode(12, INPUT); //온도 down 버튼
  pinMode(13, INPUT); //온도 up 버튼

  //서브 모터
  myServo.attach(11);
  myServo.write(0);


  digitalWrite(8,1); //초기에 온도가 너무 낳으면 status가  true값이 되지 않아서 작동을 안함.

}

void loop() {
  
  //1초 

if(millis()-timeVal>=1000){
   d1++; 
   timeVal=millis();
  }
  if(d1==60){ //초
    d1=0;
    d2++; 
  }
  if(d2==60){ //분
    d2=0;
    d3++;    
  }
  if(d3 == 3){ //시 //나는 전란을 위해 3시간 단위로 구동하도록 만듬. 
    d1 =0;
    d2 =0;
    d3 =0;
    timeVal = 0; //오버플로우방지


    //외부에 모터를 달고 전란을 할 수 있는 구조로 만든다면 이렇게 자동으로 굴릴 수 도 있음. 대신 모터의 힘이 강해야 함.
// if(myServo.read()==0){
//       for(int i =0; i<=180; i++){
//         if(millis()-timeVal>=50){ //천천히 회전시키기 위해 50으로 함. 숫자를 늘리면 더 천천히 돌릴 수 있음.
//         myServo.write(i);
//         }
//       }
//     }
//     else if(myServo.read()==180){
//       for(int i =180; i>=0; i--){
//         if(millis()-timeVal>=50){
//         myServo.write(i);
//         }
//       }
//     }
//모터부분을 부화기에 만들지 안더라도 시간을 재는 부분은 유용함.
  }
    
  
  

float  t = dht.readTemperature(); //온도값을 대입
//오차를 걷어내는 것. //비교적 저렴한 온습도계의 경우 온도 오차가 심함. 실제로 17도인데 -17도, 0도로 나오는 경우도 많음.
if(t<=0){
  status = false;
} else if(t>0){
  status = true;
}

 if(t>o+1&&status==true){ //중간 단계(36~38)를 두면 굳이 차갑게 만들 필요는 없지만 여름이나 따뜻해지는 시기에는 필요할 수 도 있음.
    analogWrite(5,255); // 핀번호, 전압 5V는 255 - 8bit 출력이라그럼 
    analogWrite(6,0);
    digitalWrite(8,0); // 릴레이 제어 - 중단
  } else if(t>=o-1&&t<=o+1&&status == true){ //온도 안정 범위 - +-1  //유정란 부화온도는 36~38도가 적당함.
    digitalWrite(8,0);
    analogWrite(5,0);  //모든 기기 끄기.
    analogWrite(6,0);
  }else if(t<o-1&&status == true){
    digitalWrite(8,1);
    analogWrite(5,0);
    analogWrite(6,0);
  }

  //나는 스티로폼 박스에 환기팬과 백열전구로 온도를 조절할 수 있게 했다.
  //왜냐하면 온습도측정이 정확하지 않아서 백열전구로 가열할때 쉽게 과열이 될 수 있어서 환기팬을 달았다.

  //만약 정밀한 온습도 측정체계를 만들 수 있다면 환기팬은 달지 않는것이 좋다. 그 이유는 환기팬을 달면 위와 같은 이유에서는 온도를 조절하기 쉽지만 온도가 너무 잘 빠져나가서 
  //백열전구가 켜졌다, 꺼졌다하는 주기가 너무 짧아서 전력손실이 발생한다.



  int  h = dht.readHumidity();   //습도값을 대입
  // Serial.print("Temperture: ");
  // Serial.print(t);
  // Serial.print(" C ");
  // Serial.print("Humidity: ");
  // Serial.print(h);
  // Serial.println(" %");



  //버튼 작동부
  //버튼 작동부를 회로에서 만들때에는 저항을 달아주는 것이 중요하다.
  //왜냐하면 외부자기장에 의해서  버튼이 입력된것 처럼 작동할 수 있기 때문이다.

  int btState_down = digitalRead(12);
  
  if(btState_down == HIGH){
    if(state_down == 0){
     
      state_down =1 ;
     
    }
  }
  if(btState_down == LOW){
    if(state_down==1){
      o-=0.5;
     
      state_down= 0;
     
    }
  }



  int btState_up = digitalRead(13);
  if(btState_up == HIGH){
    if(state_up == 0){
     
      state_up =1 ;
     
    }
  }
  if(btState_up == LOW){
    if(state_up==1){
      o+=0.5;
    
      state_up= 0;
      
    }
  }






  //lcd 구현부
  lcd.init(); //초기화
  lcd.backlight();

  //온도
  lcd.setCursor(0,0);
  lcd.print("T: ");
  lcd.setCursor(2,0);
  lcd.print((int)t);

  //습도
  lcd.setCursor(7,0);
  lcd.print(" H: ");
  lcd.setCursor(10,0);
  lcd.print(h);

  //설정온도
  lcd.setCursor(0,1);
  lcd.print("S: ");
  lcd.setCursor(2,1);
  lcd.print(o);

// 측정된 시간
  lcd.setCursor(8,1);
  lcd.print(d3);
  lcd.print(":");
  lcd.print(d2);
  lcd.print(":");
  lcd.print(d1);

}

//더 개발하면 시중에서 파는 부화기보다 더 편리한 부화기를 만들 수 있다.
//입란만 시키면 21일차까지 알아서 부화를 시켜주는 것도  가능하다.
//하지만 부화기를 만드는 것보다 더 중요한 것은 조류와 부화에 대해 학습하고 그 특성을 고려하여 만드는 것이다.
//이 과정에서는 여러번의 시행착오가 발생할 수 있다. 두려워하지 말고 도전해야한다.

  

