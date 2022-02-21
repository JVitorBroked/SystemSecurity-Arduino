// C++ code
//

#include <LiquidCrystal.h>
#include <Keypad.h>



//	convencao codEvent
// 0: alarme presenca
// 1: alarme incendio
// 2: senha errada
// 3: senha correta, alarme presenca desativado
// 4: alarme incendio desativado.
// 5: Modo 1 ativado
// 6: Modo 2 ativado


int inPIR = 10;
int outPWMAlarme = 11;
int tmpTemp = A0;
float refTmp = 5.0;
int operationMode = 1;			//define o modo de operacao atual
int tempLimit = 50;				//Temperatura limite para acionar alarme


LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

const byte qLinhas = 4;  //quantidade de linhas do teclado
const byte qColunas = 4; //quant. colunas teclado

//mapeamento das teclas
char teclas[qLinhas][qColunas] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte pinosLinhas[qLinhas] = {A1, A2, A3, A4};  // pinos usados pelas linhas teclado 
byte pinosColunas[qColunas] = {A5, 8, 12, 9}; // pinos usados pelas colunas teclado

Keypad mykeypad = Keypad(makeKeymap(teclas), pinosLinhas, pinosColunas, qLinhas, qColunas);


bool EnterPassword(){
  
  char myPassword[7] = "123456";
  char pw[7] = {};
  int qtdKey = 0;
  while(qtdKey < 6){
  	char key = mykeypad.getKey();
  	if(key){
  	  pw[qtdKey++] = key;
      lcd.print(key);
  	}
  }
  pw[6] = '\0';
 
  if(strcmp(pw, myPassword) == 0){
    sendInfoLog(3);
  	return false;
  }else{
    sendInfoLog(2);
	return true;
  }
}


void ligarSirene(){
  
  for(int x=0;x<180;x++){
  	float seno=(sin(x*3.1416/180));
  	int frequencia = 2000+(int(seno*1000));
 	tone(outPWMAlarme,frequencia);
  	delay(2);
  }
  
  if(operationMode == 1){
    sendInfoLog(0);
  	bool sireneOn = true;
  	delay(1000);
  	while(sireneOn){
  		lcd.clear();
  		lcd.setCursor(0,0);
  		lcd.print("Password:");
  		lcd.setCursor(0,1);
  
    	sireneOn = EnterPassword();
    	lcd.clear();
  	}
    sendInfoLog(3);
  }else{
	sendInfoLog(1);
    lcd.clear();
  	lcd.setCursor(0,0);
  	lcd.print("ANY KEY STOP");
  	while(!mykeypad.getKey()){}
  	sendInfoLog(4);
  }
  noTone(outPWMAlarme);
  
}

void sendInfoLog(int codEvent){
	
  
 	switch (codEvent){
 	case 0:
    	Serial.println("Alarme acionado, o sensor detectou a presenca de algo.");
    	break;
  	case 1:
    	Serial.println("Alarme acionado, temperatura maior que: ");
      	Serial.print(tempLimit);
      Serial.println("C");
    	break;
    case 2:
    	Serial.println("Senha incorreta.");
    	break;
    case 3:
    	Serial.println("Senha Correta, Alarme desativado.");
    	break;
    case 4:
    	Serial.println("Alarme incedio desativado.");
    	break;
    case 5:
    	Serial.println("Modo 1 foi ativado.");
    	break;
    case 6:
    	Serial.println("Modo 2 foi ativado.");
    	break;
	}
  
	Serial.print("Tempo passado da ativacao do sistema a ocorrencia do evento: ");
	Serial.print(millis()/1000);
    Serial.print(" s.\n");
}

int selectModeOperation(){
  	lcd.clear();
	lcd.setCursor(0,0);
  	lcd.print("OPERATION MODE");
  	lcd.setCursor(0,1);
  	lcd.print("SPACE=1 FIRE=2");
  
  	while(true){
  		char mode = mykeypad.getKey();
      	if(mode == '1'){
          	lcd.clear();
          	lcd.setCursor(0,0);
      		lcd.print("Mode 1 active");
        	sendInfoLog(5);
         	delay(2500);
          	return 1;
        }else if(mode == '2'){
        	lcd.clear();
          	lcd.setCursor(0,0);
      		lcd.print("Mode 2 active");
          	sendInfoLog(6);
            delay(2500);
            return 2;
        } 
  	}
}

void setup()
{
  lcd.begin(16, 2);
  Serial.begin(9600);
  pinMode(inPIR, INPUT);
  pinMode(outPWMAlarme, OUTPUT);
  pinMode(tmpTemp, INPUT); 			//pino A0 usado pelo sensor tmp
  pinMode(LED_BUILTIN, OUTPUT);
  
  operationMode = selectModeOperation();
  
}

void loop()
{	
  
  //Determina no modo de operacao selecionado
  if(operationMode == 1){
  	lcd.clear();
  	lcd.setCursor(3, 0);
    lcd.print("ALARM ON");
    //verificar estado do sensor de pir
  	if(digitalRead(inPIR) == HIGH){		//se detectado movimento ativar piezo(alarme)
  		lcd.clear();
  		lcd.setCursor(0, 0);
    	lcd.print("ALARME ACIONADO");
    	ligarSirene();
  	}else{
  		analogWrite(outPWMAlarme, 0);
  	}
    delay(100);
  // Se modo de incedio ativo, executa instrucoes abaixo  
  }else{
  	
    float voltageNow = analogRead(tmpTemp);
  	float nowtempC = (((voltageNow/1024.0) * refTmp) - .5) * 100; 
  	
    lcd.clear();
  	lcd.setCursor(0, 0);
    lcd.print("TEMP NOW: ");
    lcd.print(nowtempC);

  	if(nowtempC >= tempLimit){
  	  lcd.print("ALARME ACIONADO");
      ligarSirene();
  	}
  	
  }
    
  delay(500);
  
}