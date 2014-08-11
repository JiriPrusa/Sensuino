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
 * @author Jointar Ganjar
 */
abstract class TabPanel extends JPanel implements ItemListener, ActionListener {
   JPanel cards;
   JToggleButton uploadButton;
   int pinNumber;
   JComboBox cb;
   JPanel comboBoxPane = new JPanel();
   
   public TabPanel(int pin) {
    this.setLayout(new BoxLayout(this, BoxLayout.Y_AXIS) );
      pinNumber=pin;
      uploadButton=new JToggleButton("set");
      uploadButton.addActionListener(this);
      uploadButton.setAlignmentX(Component.CENTER_ALIGNMENT);
      
      }
   
    public abstract void retype();
    
    public abstract void reconst();
    
    public abstract void addValue(String inVal);
    
       public void untoggleButton(){
    uploadButton.setSelected(false);
    }
       

        @Override
     public void itemStateChanged(ItemEvent evt) {
       if(SensApl.getChangeCard()){
     
        CardLayout cl = (CardLayout)(cards.getLayout());
        cl.show(cards, (String)evt.getItem());
       }
       
       
    }

          
}
