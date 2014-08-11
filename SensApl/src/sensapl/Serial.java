/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package sensapl;
import java.io.InputStream;
import java.io.OutputStream;
import gnu.io.CommPortIdentifier; 
import gnu.io.SerialPort;
import gnu.io.SerialPortEvent; 
import gnu.io.SerialPortEventListener; 

import java.util.Enumeration;
import java.text.ParseException;
import java.util.Date;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.io.DataOutputStream;
import java.util.ArrayList;
/**
 *
 * @author Jointar Ganjar
 */
public class Serial implements SerialPortEventListener {
 boolean DEBUG = true;
    
    SerialPort serialPort;
         
      //public static  PipedInputStream pin=new PipedInputStream();
        
       boolean firstContact;
       boolean dataReaded;
       int jedna=50;
       private static int[] analogType=new int[3];
       private static int[] digitalType=new int[4];
       private static int[] I2CType=new int[1];
       
       private static float[][] PHconst=new float[3][2];
       private static float[][] ECconst=new float[4][2];
       
       
       
      // private ArrayList<String> D1val = new ArrayList<String>();
      //  ArrayList<String> D2val = new ArrayList<String>();
       // ArrayList<String> D3val = new ArrayList<String>();
      //  ArrayList<String> D4val = new ArrayList<String>();
      //  ArrayList<String> A1val = new ArrayList<String>();
      //  ArrayList<String> A2val = new ArrayList<String>();
      //  ArrayList<String> A3val = new ArrayList<String>();
       // ArrayList<String> I2Cval = new ArrayList<String>();
       
       
       String D1val;
       String D2val;
       String D3val;
       String D4val;
       String A1val;
       String A2val;
       String A3val;
       String I2Cval;
       
       public static int[] getAnalogType(){
       return analogType;
       }
       
       public static int[] getDigitalType(){
       return digitalType;
       }
       
       public static int[] getI2CType(){
       return I2CType;
       }
       
       public static Float getPHconst(int firstIndx, int secondIndx){
       return PHconst[firstIndx][secondIndx];
       }
       
       public static Float getECconst(int firstIndx, int secondIndx){
       return ECconst[firstIndx][secondIndx];
       }
       
      
       
        /** The port we're normally going to use. */
	private static final String PORT_NAMES[] = { 
			"/dev/tty.usbserial-A9007UX1", // Mac OS X
			"/dev/ttyUSB0", // Linux
                        "/dev/ttyACM0",  //pripsano
			"COM5", // Windows
			};
	/** Buffered input stream from the port */
	private InputStream input;
	/** The output stream to the port */
	private OutputStream output;
        
        private DataOutputStream dataOut;
              
	/** Milliseconds to block while waiting for port open */
	private static final int TIME_OUT = 2000;
	/** Default bits per second for COM port. */
	private static final int DATA_RATE = 9600;

        public String [] portEnum(){
        Enumeration portEnum = CommPortIdentifier.getPortIdentifiers();
        int i=0;
        String [] ports=new String [0];
        while (portEnum.hasMoreElements()) {
        CommPortIdentifier currPortId = (CommPortIdentifier)portEnum.nextElement();
        if(currPortId.getPortType()==CommPortIdentifier.PORT_SERIAL){
        ports = java.util.Arrays.copyOf(ports, ports.length + 1);
        ports[i]=currPortId.getName(); 
        i++;
        }
        }
        //System.out.println(i);
        //System.out.println(ports.length);
        for(int j=0; j<ports.length; j++){
        System.out.println(ports[j]);
                }
        return ports;
        }

	public void initialize() {
 
            
            
		CommPortIdentifier portId = null;
		Enumeration portEnum = CommPortIdentifier.getPortIdentifiers();

		// iterate through, looking for the port
		/*
                while (portEnum.hasMoreElements()) {
			CommPortIdentifier currPortId = (CommPortIdentifier) portEnum.nextElement();
			for (String portName : PORT_NAMES) {
				if (currPortId.getName().equals(portName)) {
					portId = currPortId;
					break;
				}
			}
		}*/
                
                while (portEnum.hasMoreElements()) {
			CommPortIdentifier currPortId = (CommPortIdentifier) portEnum.nextElement();
			//for (String portName : PORT_NAMES) {
				if (currPortId.getName().equals(SensApl.portSelecter.getSelectedItem())) {
					portId = currPortId;
					break;
				}
                }
			
               
		if (portId == null) {
			System.out.println("Could not find COM port.");
			return;
		}

		try {
			// open serial port, and use class name for the appName.
			serialPort = (SerialPort) portId.open(this.getClass().getName(),
					TIME_OUT);

			// set port parameters
			serialPort.setSerialPortParams(DATA_RATE,
					SerialPort.DATABITS_8,
					SerialPort.STOPBITS_1,
					SerialPort.PARITY_NONE);

			// open the streams
			input = serialPort.getInputStream();
			output = serialPort.getOutputStream();
                        dataOut=new DataOutputStream(serialPort.getOutputStream());
			// add event listeners
			serialPort.addEventListener(this);
			serialPort.notifyOnDataAvailable(true);
		} catch (Exception e) {
			System.err.println(e.toString());
		}
              
	}

	/**
	 * This should be called when you stop using the port.
	 * This will prevent port locking on platforms like Linux.
	 */
	public synchronized void close() {
		if (serialPort != null) {
			serialPort.removeEventListener();
			serialPort.close();
                        System.out.println("Port close");
		}
	}

	/**
	 * Handle an event on the serial port. Read the data and print it.

         
         */
        
 public void startComunication(){
initialize();
try{
Thread.sleep(TIME_OUT);
}catch(Exception e){

}
sendByte((byte)('Y'));  

System.out.println("starting comunication"); 
 }
 
 public void stopComunication(){
 if(firstContact){
 sendByte((byte)('X'));
 System.out.println("comunication stoped");
 firstContact=false;
 close();
 }
 
 }
        
public String valueRead(){
System.out.println("valueRead > zacinam cist");
char[] ECarr=new char[11];
int p=0;
char ECchar;
//while((ECchar=readByte())!='Z'){
while((ECchar=readByte())!='\n'){ 
if(DEBUG) System.out.println("Reading "+p+" char="+ECchar);
ECarr[p]=ECchar;
p++;
}
String ECstring=((new String(ECarr)).substring(0,p));
if(DEBUG) System.out.println("valueRead > Hodnota:"+ECstring);
return ECstring;
}
/*
public String condRead(){
System.out.println("zacinam cist");
char[] ECarr=new char[20];
int p=0;
char ECchar;
while((ECchar=readByte())!='Z'){ 
ECarr[p]=ECchar;
p++;
}
String ECstring=((new String(ECarr)).substring(0,p));
System.out.println("Hodnota:"+ECstring);
return ECstring;
}
*/

         
public char readByte (){
try {
return(char)input.read();
}catch (Exception e) {
System.err.println(e.toString());
return 'N';
}

}

public void sendByte(byte myByte){
			try {
				output.write(myByte);
                                //System.out.println(myByte);
				output.flush();
                                
                                
			} catch (Exception e) {
				System.err.println(e.toString());
			}
		}

public void sendTime(){
    int[] send;
    int unixTime=0;
    if(!SensApl.timeEdited){
	                  unixTime=(int) (System.currentTimeMillis() / 1000L);
                          }else{
        try {
            String timeToSend=SensApl.time.getText();
            Date upgraded=SensApl.dateFormat.parse(timeToSend);
            System.out.println("Prekladam datum");
            unixTime=(int)(upgraded.getTime()/ 1000L);
            
        } catch (ParseException ex) {
            Logger.getLogger(Serial.class.getName()).log(Level.SEVERE, null, ex);
           
        }       
    }
    send=convertInt2Array(unixTime);
    
                          try {
                              System.out.println("-----------------> "+send.length);
                              for (int i=0; i < 10-(send.length);i++){
                              output.write(0);
                              System.out.println("posilam: 0");
                              }
                          for (int i = 0; i < send.length; i++) {
                                output.write(send[i]);
                                    //sendByte((char)send[i]);
                                   //System.out.println(i+": "+send[i]);
                              System.out.println("posilam:"+send[i]);
                            }
                                                     
			} catch (Exception e) {
				System.err.println(e.toString());
			}
  
}


public void sendECcontstants(int num, float a,float b) throws Exception{
sendByte((byte)('U'));
sendByte((byte)num);
dataOut.writeUTF(String.valueOf(a));
dataOut.writeUTF(String.valueOf(b));
}

public void sendPHcontstants(int num, float a,float b) throws Exception{
sendByte((byte)('V'));
sendByte((byte)num);
dataOut.writeUTF(String.valueOf(a));
dataOut.writeUTF(String.valueOf(b));
}

public void calibrationECstart(int num) {
sendByte((byte)('W'));
sendByte((byte)num);
}

public void calibrationPHstart(int num) {
sendByte((byte)('W'));
sendByte((byte)num);
}

public void calibrationStop() {
sendByte((byte)('W'));
}

public void getTime(){
sendByte((byte)('A'));  
System.out.println("callTime");
}


/*
public void getECcontstants(int num){
sendByte((byte)('U'));
sendByte((byte)num);
System.out.println("callEC"+num+"contstants");
}

public void getPHcontstants(int num){
sendByte((byte)('V'));
sendByte((byte)num);
System.out.println("callPH"+num+"contstants");
}
*/

public void getECpure(int pin){
sendByte((byte)('M'));
sendByte((byte)pin);
System.out.println("call getECpure on pin "+pin);
}

public void getPHpure(int pin){
sendByte((byte)('N'));
sendByte((byte)pin);
System.out.println("call getECpure on pin"+pin);
}

public void sendAnalogType(int pin, int type){
sendByte((byte)('T'));
sendByte((byte)('A'));
sendByte((byte)(pin+1));
System.out.println("sending analog type: "+pin+" "+type);
sendByte((byte)(type));
}

public void sendDigitalType(int pin, int type){
sendByte((byte)('T'));
sendByte((byte)('D'));
sendByte((byte)(pin+1));
System.out.println("sending digital type: "+pin+" "+type);
sendByte((byte)(type));
}

public void sendI2CType(int pin, int type){
sendByte((byte)('T'));
sendByte((byte)('I'));
sendByte((byte)(pin+1));
System.out.println("sending i2c type: "+(pin+1)+" "+type);
sendByte((byte)(type));

}

public void setDelay(){
    try{
        sendByte((byte)'D');
        System.out.println("nastavuji delay !!!!!!!!!!!! " + SensApl.delay.getText());
    dataOut.writeUTF(SensApl.delay.getText());
    }catch (Exception e) {
        System.out.print("Chyba v metode setDelay-> ");
        System.err.println(e.toString());
    }
}

public void setTime(){
try{
sendByte((byte)'B');
Thread.sleep(500L);    // one second
sendTime();
System.out.println("callSetTime");
    }catch (Exception e) {
        System.err.println(e.toString());
    }
}

public static int[] convertInt2Array(int guess)  {
    String temp = Integer.toString(guess);
    
    String[] sNums = Integer.toString(guess).split("");
    char c;
    int j;
    int pole []=new int[temp.length()]; 
    for (int i = 0; i < temp.length(); i++) {
        c=temp.charAt(i);
        j = c - '0';
       pole[i]=j;
       }
     return pole;  
}

	public synchronized void serialEvent(SerialPortEvent oEvent) {
		if (oEvent.getEventType() == SerialPortEvent.DATA_AVAILABLE) {
		
                    try{
                    //String inputLine=Binput.readLine();
			//	System.out.println("IN: "+inputLine);
                    // A-zacatek
                    while(input.available()>0){
                        //if(!dataReaded){
                       int inByte=input.read();
                        System.out.println("input char: "+(char)inByte);
                     if (!firstContact) {
                         
    if ((char)inByte == 'Y') {
                // clear the serial port buffer
      firstContact = true;     // you've had first contact from the microcontroller
     SensApl.setConectionLabel(true);
      System.out.println("contact established");
      // sendByte((byte)'Y');       // ask for more
    }
    }else{

if((char)inByte=='A'){
SensApl.takeDate(Long.parseLong(valueRead(), 10));
System.out.println("time readed");
}

if((char)inByte=='D'){
SensApl.takeDelay(Long.parseLong(valueRead()));

} 


if((char)inByte=='T'){
    System.out.println("Types readed");
    while(input.available()<10){
    //System.out.println("malo dat");
    };

    if((char)input.read()=='A'){
        analogType[0]=Integer.valueOf(valueRead());
        analogType[1]=Integer.valueOf(valueRead()); 
        analogType[2]=Integer.valueOf(valueRead());
        I2CType[0]=Integer.valueOf(valueRead());  //char 4
   
    }else{
        digitalType[0]=Integer.valueOf(valueRead()); //char 1
        digitalType[1]=Integer.valueOf(valueRead()); //char 2
        digitalType[2]=Integer.valueOf(valueRead()); //char 3
        digitalType[3]=Integer.valueOf(valueRead()); //char 4
        } 
  
  SensApl.revalidateButtons(); 
 
  SensApl.revalidatePanels();
}

if((char)inByte=='C'){
    System.out.println("Constants readed");
    while(input.available()<2){};  //get sure if there is enough bytes to read from
    if((char)input.read()=='P'){
        PHconst[0][0]=Float.valueOf(valueRead()); PHconst[0][1]=Float.valueOf(valueRead());    //char 1
        PHconst[1][0]=Float.valueOf(valueRead()); PHconst[1][1]=Float.valueOf(valueRead());    //char 2
        PHconst[2][0]=Float.valueOf(valueRead()); PHconst[2][1]=Float.valueOf(valueRead());    //char 3
        
    }else{
        
        //ECconst[0][0]=Integer.valueOf(valueRead()); ECconst[0][1]=Integer.valueOf(valueRead());    //char 1
        ECconst[1][0]=Float.valueOf(valueRead()); ECconst[1][1]=Float.valueOf(valueRead());   //char 2
        ECconst[2][0]=Float.valueOf(valueRead()); ECconst[2][1]=Float.valueOf(valueRead());  //char 3
        ECconst[3][0]=Float.valueOf(valueRead()); ECconst[3][1]=Float.valueOf(valueRead()); //char 4
      } 
    SensApl.revalidateConstants();
}



if((char)inByte=='M'){
System.out.print("EC clear readed on pin ");
  
SensApl.upgradeECsignal(input.read(),Float.valueOf(valueRead())); 

}

if((char)inByte=='N'){
System.out.print("pH clear readed on pin ");

SensApl.upgradePHsignal(input.read(),Float.valueOf(valueRead())); 

}


if((char)inByte=='V'){
System.out.println("Values reading...");
while(input.available()<40){};
//D1val=valueRead();
D2val=valueRead();
D3val=valueRead();
D4val=valueRead();
A1val=valueRead();
A2val=valueRead();
A3val=valueRead();
I2Cval=valueRead();
System.out.println("Values readed");
SensApl.revalidateValues(D2val, D3val, D4val, A1val, A2val, A3val, I2Cval);
}
  
    
if((char)inByte=='Z'){  
    System.out.println("Precet sem Z");
System.out.println(D2val);
System.out.println(D3val);
System.out.println(D4val);
System.out.println(A1val);
System.out.println(A2val);
System.out.println(A3val);
System.out.println(I2Cval);
SensApl.revalidateValues(D2val, D3val, D4val, A1val, A2val, A3val, I2Cval);
    
/*    
System.out.println(analogType[0]);
System.out.println(analogType[1]);
System.out.println(analogType[2]);
System.out.println(I2CType[0]);
SensApl.revalidatePanels();
*/
    /*
System.out.println(PHconst[0][0]);
System.out.println(PHconst[0][1]);
System.out.println(PHconst[1][0]);
System.out.println(PHconst[1][1]);    
System.out.println(PHconst[2][0]);
System.out.println(PHconst[2][1]);
 */ 
}


/*
try{
//if(Viewer.plotPanel.ECButton.isSelected())Viewer.plotPanel.plot.addToGraph((int)(Float.parseFloat(value)*10));
}catch(Exception e){
    System.out.println("Chyba"+e.toString());}
} 
*/


/*
if((char)bytek=='F'){
    int pin=(input.read());
System.out.println("Freq readed on pin "+pin);
Sensuino.setFreqpure(pin, valueRead());
} 

if((char)bytek=='G'){
System.out.println("claibrateables readed");
int times=input.read()-48;
for (int i = 0; i < times; i++) {
String value=valueRead();        
Calibration.calibrateableList.add(value);
}
Sensuino.calibration.reArange();
System.out.println("KONEC");
}

if((char)bytek=='K'){
System.out.println("conditions readed");
int times=input.read()-48;
//    System.out.println("podminkakrat "+times);
for (int i = 0; i < times ; i++) {
String value=condRead();        
Sensuino.conditionsList.add(value);
//    System.out.println("ctu po"+i);
    
}
Viewer.conditionsPanel.setText(Viewer.getConditionsString());
}

  
if((char)bytek=='W'){
    int pin=(input.read());
    System.out.println("Wtemp readed on pin "+pin);
       switch(pin){
        case 49:Viewer.valA1.setCaption("WTemp_1");Viewer.valA1.setValue(valueRead()+"°C"); break;     //char 1
        case 50:Viewer.valA2.setCaption("WTemp_2");Viewer.valA2.setValue(valueRead()+"°C"); break;    //char 2
        case 51:Viewer.valA3.setCaption("WTemp_3");Viewer.valA3.setValue(valueRead()+"°C"); break;  //char 3
      }
//if(Viewer.plotPanel.CO2Button.isSelected())Viewer.plotPanel.plot.addToGraph((int)(Float.parseFloat(value)*10));

}     
*/  
 
System.out.println("reach End");
//System.out.println(valueRead());
}

 /*   
if((char)bytek=='X'){                        /////mimmo
System.out.print((valueRead()));
System.out.println("obdrzeno ");                      
}
*/                                         
                     }
                   // } FROM DATA READED
                          
                }catch(Exception e){
                System.out.println("Chyba"+e.toString());
                }
                	
              
                }	   
}
}
