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
public class I2CTabPanel extends TabPanel {
   
    final static String naPANEL= "N/A";
    final static String lightPANEL = "light intesity";
    final static String timePANEL = "time";
  
   Panel cardLIGHT;
           
   Panel cardTIME;
    
    public I2CTabPanel(int pin) {
        super(pin);
        
        //Put the JComboBox in a JPanel to get a nicer look.
        String comboBoxItems[] = { naPANEL, lightPANEL, timePANEL };
        cb = new JComboBox(comboBoxItems);
        cb.setEditable(false);
        cb.addItemListener(this);
        comboBoxPane.add(cb);
         
        //Create the "cards".
        JPanel cardLIGHT = new ClassicPanel(lightPANEL);
    
        JPanel cardTIME = new ClassicPanel(timePANEL);

        //Create the panel that contains the "cards".
        cards = new JPanel(new CardLayout());
        cards.add(new JPanel(), naPANEL);
        cards.add(cardLIGHT, lightPANEL);
        cards.add(cardTIME, timePANEL);
        
        //cb.setSelectedIndex(Serial.getI2CType()[pinNumber]);
        
        this.add(comboBoxPane);
        this.add(uploadButton);
        this.add(cards);
    }
    
    @Override
     public void retype(){
        cb.setSelectedItem(null);
    cb.setSelectedIndex((Serial.getI2CType()[pinNumber])-1);
   
    }
    
    @Override
    public void reconst(){
        System.out.println("I2C panel no calib. const needed");
    }
 
     @Override
    public void addValue(String inVal){
         System.out.println("Selected: " + cb.getSelectedIndex());  
    
            switch(cb.getSelectedIndex()){
            case 1: cardLIGHT.setValue(inVal);break;
            case 2: cardTIME.setValue(inVal);break;
                       
        }
    }
    
   
    
    @Override
         public void actionPerformed(ActionEvent evt) {
        SensApl.mainSerial.sendI2CType(pinNumber, cb.getSelectedIndex()+1 );
}
    
    
    
}

