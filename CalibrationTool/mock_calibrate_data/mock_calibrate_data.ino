
unsigned int counter = 0;

void setup() {
  Serial.begin(9600);
  delay(1000);
  randomSeed(analogRead(A0));
  
}

void loop() {

  if (counter == 160) // send the sample count and return
  {
    Serial.println("AVERAGE_SAMPLE_COUNT");
    unsigned int sampcnt = random(400, 700);
    Serial.println(sampcnt);

    while(true); // loop forever at end
  }
    
  if (counter == 0)
    Serial.println("ALUMINIUM");

  if (counter == 40)
    Serial.println("STEEL");

  if (counter == 80)
    Serial.println("WHITE");

  if (counter == 120)
    Serial.println("BLACK");

  unsigned int sendval = 0;
  
  if (counter < 40) // Aluminium
    sendval = random(10, 100);
  else if (counter < 80) // Steel
    sendval = random(400, 600);
  else if (counter < 120) // white
    sendval = random(850, 950);
  else if (counter < 160) // black
    sendval = random(900, 1000);

  Serial.println(sendval);
  
  counter++;
  delay(50);
}
