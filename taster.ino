// cndrbrbr, 2024
// LED Intern Taster an Pin 6

int TasterPin = 7;   

// speichert den Status der LED: 0 = aus, 1 = ein
int StatusTaster = 0;  
int secs = 0;

void Blink (int duration,int times){
  for (int i = 0;i<times;i++){
      digitalWrite(LED_BUILTIN, HIGH); // LED-Spannung wird auf High gesetzt, die LED leuchtet.
      delay(duration); // Wartezeit in Millisekunden - die LED leuchtet weiterhin.
      digitalWrite(LED_BUILTIN, LOW); // LED-Spannung wird auf LOW gesetzt, die LED erlischt.
      delay(duration); // Wartezeit in Millisekunden - die LED ist weiterhin aus.
  }
}
void setup()  
{
  // LED wird als OUTPUT (Ausgang) definiert
  pinMode(LED_BUILTIN, OUTPUT); // D13 (LED_BUILTIN) wird als Output festgelegt.
 
  // der Taster wird als INPUT (Eingang) deklariert
  pinMode(TasterPin, INPUT);  
}

void loop() 
{  
  //secs = millis();
  // Taster abfragen
  StatusTaster = digitalRead(TasterPin); 

  // wenn der Taster gedrückt wurde
  if (StatusTaster == HIGH) 
  {      
    // LED für 5 Sekunden (5000 ms) einschalten                         
    Blink (1000,1);

  }                               
}
