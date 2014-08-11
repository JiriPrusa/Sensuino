/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package sensapl;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
/**
 *
 * @author Jirka
 */
public class DigitalTabPanel extends TabPanel {
  
    final static String naPANEL= "N/A";
    final static String dht11PANEL = "DHT11";
    final static String dht22PANEL = "DHT22";
    final static String ecPANEL = "EC";
    final static String distancePANEL = "Ultrazvuk";
    final static String waterTemp = "water temp";
   // final static String soilPANEL = "soil humidity";
           
    
        
        Panel cardDHT11;
    
        Panel cardDHT22; 
       
        CalibrateablePanel cardEC; 
        
        Panel cardDISTANCE; 
        
        Panel cardSOIL;
   
        Panel cardWaterTemp;
    
    public DigitalTabPanel(int pin) {
        super(pin);
        String comboBoxItems[] = { naPANEL, dht11PANEL, dht22PANEL, ecPANEL, distancePANEL, waterTemp/*, soilPANEL */};

        cb= new JComboBox(comboBoxItems);
        cb.setEditable(false);
        cb.addItemListener(this);
        comboBoxPane.add(cb);
         

        //Create the "cards".
         cardDHT11 = new DHTPanel(dht11PANEL);
    
         cardDHT22 = new DHTPanel(dht22PANEL);
       
         cardEC = new CalibrateablePanel(ecPANEL, pinNumber);
        
         cardDISTANCE = new ClassicPanel(distancePANEL); 
         
         cardWaterTemp = new ClassicPanel(waterTemp); 
        
         //cardSOIL = new ClassicPanel(soilPANEL);

        //Create the panel that contains the "cards".
        cards = new JPanel(new CardLayout()); 
        cards.add(new JPanel(), naPANEL); //0
        cards.add(cardDHT11, dht11PANEL); //1
        cards.add(cardDHT22, dht22PANEL); //2
        cards.add(cardEC, ecPANEL); //3
        cards.add(cardDISTANCE, distancePANEL); //4
        cards.add(cardWaterTemp, waterTemp); //5
       // cards.add(cardSOIL, soilPANEL); //6
        
        //cb.setSelectedIndex(Serial.getDigitalType()[pinNumber]);
        
        this.add(comboBoxPane);
        this.add(uploadButton);
        this.add(cards);
    }
    
    @Override
     public void retype(){
       
    cb.setSelectedItem(null);
    cb.setSelectedIndex((Serial.getDigitalType()[pinNumber])-1);
    //  ItemEvent evt=new ItemEvent
    
        }
    
     @Override
      public void reconst(){
      cardEC.setCalibrationConstants();  
    }
    
    @Override
    public void addValue(String inVal){
    System.out.println("Selected: " + cb.getSelectedIndex());  
    
        switch(cb.getSelectedIndex()){
            case 1: cardDHT11.setValue(inVal);break;
            case 2: cardDHT22.setValue(inVal);break;
            case 3: cardEC.setValue(inVal);break;
            case 4: cardDISTANCE.setValue(inVal);break;
            case 5: cardWaterTemp.setValue(inVal);break;    
        }
        
  
    }
     
 
    @Override
        public void actionPerformed(ActionEvent evt) {
        SensApl.mainSerial.sendDigitalType(pinNumber, cb.getSelectedIndex()+1);
}
}

