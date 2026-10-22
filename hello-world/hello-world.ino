void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  int i=0;
  Serial.println("Hello world!");
  Serial.println(i++);
  
}
