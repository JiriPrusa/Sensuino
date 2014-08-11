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
public class AnalogTabPanel extends TabPanel {
   
    final static String naPANEL= "N/A";
    final static String phPANEL = "pH";
    final static String co2PANEL = "CO2";
   // final static String soilPANEL = "soil humidity";
   
    public CalibrateablePanel cardPH;
    public Panel cardCO2;
    public Panel cardSOIL;
            
    public AnalogTabPanel(int pin) {
        super(pin);
       
        String comboBoxItems[] = { naPANEL, phPANEL, co2PANEL/*, soilPANEL*/ };
        cb = new JComboBox(comboBoxItems);
        cb.setAlignmentX(Component.CENTER_ALIGNMENT);
        cb.setEditable(false);
        cb.addItemListener(this);
        
        comboBoxPane.add(cb);
        
         
        //Create the "cards".
        cardPH = new CalibrateablePanel(phPANEL,pinNumber);
    
        cardCO2 = new ClassicPanel(co2PANEL);
       
        //cardSOIL = new ClassicPanel(soilPANEL);
         
         
        //Create the panel that contains the "cards".
        cards = new JPanel(new CardLayout());
        cards.add(new JPanel(), naPANEL);
        cards.add(cardPH, phPANEL);
        cards.add(cardCO2, co2PANEL);
        //cards.add(cardSOIL, soilPANEL);
        
        //cb.setSelectedIndex(Serial.getAnalogType()[pinNumber]);
        
        this.add(comboBoxPane);
        this.add(uploadButton);
        this.add(cards);
    }
    
    @Override
    public void retype(){
     cb.setSelectedItem(null);
        cb.setSelectedIndex((Serial.getAnalogType()[pinNumber])-1);
     }
    
    @Override
      public void reconst(){
      cardPH.setCalibrationConstants();  
    }
    
     @Override
    public void addValue(String inVal){
      System.out.println("Selected: " + cb.getSelectedIndex());  
    
        switch(cb.getSelectedIndex()){
            case 1: cardPH.setValue(inVal);break;
            case 2: cardCO2.setValue(inVal);break;
            case 3: cardSOIL.setValue(inVal);break;
           
        }
        
    }
     
   
   
    @Override
     public void actionPerformed(ActionEvent evt) {
         SensApl.mainSerial.sendAnalogType(pinNumber, cb.getSelectedIndex()+1);
}
    
     
    
}

