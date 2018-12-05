
unsigned int counter = 0;

void setup() {
  Serial.begin(9600, SERIAL_8N1);
  delay(1000);
  randomSeed(analogRead(A0));
  
}

void loop() {

  if (counter == 160) // send the sample count and return
  {
    Serial.print("AVERAGE_SAMPLE_COUNT\n");
    unsigned int sampcnt = random(400, 700);
    Serial.print(sampcnt);
    Serial.print('\n');
    Serial.print("END\n");

    while(true); // loop forever at end
  }
    
  if (counter == 0)
    Serial.print("ALUMINIUM\n");

  if (counter == 40)
    Serial.print("STEEL\n");

  if (counter == 80)
    Serial.print("WHITE\n");

  if (counter == 120)
    Serial.print("BLACK\n");

  unsigned int sendval = 0;
  
  if (counter < 40) // Aluminium
    sendval = random(10, 100);
  else if (counter < 80) // Steel
    sendval = random(400, 600);
  else if (counter < 120) // white
    sendval = random(850, 950);
  else if (counter < 160) // black
    sendval = random(900, 1000);

  Serial.print(sendval);
  Serial.print('\n');
  
  counter++;
  delay(50);
}
