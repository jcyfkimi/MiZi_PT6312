
// - - Functions
#define uchar unsigned char
#define uint unsigned int
uint VFD_display_ram[8];   // Memory usage：8 * 2Bytes
// - - PIN Definitions
// - - PT6312 SPI
int CLK = 10; // - - PT6312 clock pin
int DATA = 9; // - - PT1621 data pin
int STB = 8; // - - PT6312 chip select pin

void VFD_init(void)
{
  pinMode(CLK, OUTPUT);
  pinMode(DATA, OUTPUT);
  pinMode(STB, OUTPUT);
  digitalWrite(CLK, HIGH);
  digitalWrite(DATA, HIGH);
  digitalWrite(STB, HIGH);
  VFD_cmd(0x04);      //100: 8 digits, 14 segments
  VFD_clr();
  VFD_cmd(0x8f);      //display on , pulse width 14/16
}

void VFD_clr(void)
{
  uchar i;
  VFD_cmd(0x40); //00: Write data to display mode
  //The address is considered valid if it has a value of “00H” to “15H”.
  for (i = 0; i < 8; i++)
  {
    digitalWrite(STB, HIGH);
    digitalWrite(STB, LOW);
    write_6311(0xC0 + 2 * i); //Address setting cmd
    write_6311(0x00);
    write_6311(0x00);
    digitalWrite(STB, HIGH);
  }
  VFD_cmd(0x8f);      //display on , pulse width 14/16
}

//send one byte data to pt6312
void write_6311(uchar w_data)
{
  uchar i;
  for (i = 0; i < 8; i++)
  {
    digitalWrite(CLK, LOW);
    if (w_data & 0x01)
      digitalWrite(DATA, HIGH);
    else
      digitalWrite(DATA, LOW);
    digitalWrite(CLK, HIGH);
    w_data >>= 1;
  }
}

void VFD_cmd(uchar command)
{
  digitalWrite(STB, HIGH);
  digitalWrite(STB, LOW);
  write_6311(command);
  digitalWrite(STB, HIGH);
}

void VFD_led(uint str)
{
  digitalWrite(STB, HIGH);
  digitalWrite(STB, LOW);
  write_6311(0x41);
  write_6311(str);
  digitalWrite(STB, HIGH);
}

// Update the data to display(address auto incrase mode)
void VFD_update(void)
{
  uchar i;
  uchar temp_H, temp_L;

  VFD_cmd(0x40); //00: Write data to display mode
  for (i = 0; i < 8; i++)   // 8 bit
  {
    digitalWrite(STB, HIGH);
    digitalWrite(STB, LOW);
    //Serial.println( i);
    write_6311(0xC0 + 2 * i); //start address
    temp_H = (uchar)(VFD_display_ram[i] >> 8  );
    temp_L = (uchar)(VFD_display_ram[i] & 0xff);
    write_6311(temp_L);  // low 8 bit data
    write_6311(temp_H);  // high 8 bit data
    digitalWrite(STB, HIGH);
  }
  VFD_cmd(0x8f);      //display on , pulse width 14/16
}

void setup() {
  Serial.begin(9600);
  VFD_init();
  Serial.println("Ok");
}
uchar i = 0;
uint j = 0;
void loop() {
  for (i = 0; i < 3; i++) {
    VFD_led(~(1 << i) & ~(1 << 3));
    delay(500);
  }
  for (i = 0; i < 8; i++)
  {
    for (j = 0; j < 65535; j++)
    {
      VFD_display_ram[i] = j;
      Serial.print("j = ");
      Serial.println(j);
      if (i) VFD_display_ram[i - 1] = 0x0000;
      VFD_update();
      delay(1000);
    }
  }
  VFD_clr();
}
