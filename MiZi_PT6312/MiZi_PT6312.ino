
// - - Functions
#define uchar unsigned char
#define uint unsigned int
uint VFD_display_ram[8];   // Memory usage：8 * 2Bytes
uint num_array[]= {0x19a7, 0x1020, 0x30c3, 0x30e1, 0x3160, 0x21e1, 0x21e3, 0x11a0, 0x31e3, 0x31e1};
uint char_array[]={0x31e2, 0x2163, 0x0183, 0x3063, 0x21c3, 0x21c2, 0x2163, 0x3162, 0x0489, 0x1021, 0x0992, 0x0103, 0x1b22, 0x1332, 0x2063, 0x31c2,0x3133, 0x31d2, 0x21e1, 0x0488, 0x1123, 0x0906, 0x1136, 0x0a14, 0x11e1, 0x2805};
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
    for (j = 0; j < 26; j++)
    {
      VFD_display_ram[i] = char_array[j];
      Serial.print("j = ");
      Serial.println(j);
      if (i) VFD_display_ram[i - 1] = 0x0000;
      VFD_update();
      delay(500);
    }
  }
  VFD_clr();
}
