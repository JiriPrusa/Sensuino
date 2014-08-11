/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package sensapl;

import java.awt.Color;
import javax.swing.*;
import java.awt.event.MouseListener;
import java.awt.event.MouseEvent;
import java.awt.Component;
import javax.swing.BorderFactory;
/**
 *
 * @author Jirka
 */
public class SensButton extends JPanel implements MouseListener{
JLabel label=new JLabel();
JLabel type=new JLabel();
JLabel value=new JLabel("N/A");
String pinLabel;
int panelNumber;

int tabNumber;
public SensButton(int panel, String name){
this.setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
this.setBorder(BorderFactory.createRaisedBevelBorder());
tabNumber=panel;
pinLabel=name;
panelNumber=panel;
type.setText(returnType());
label.setText(name);

this.value.setBorder(BorderFactory.createEmptyBorder(10,10,10,10));

this.label.setAlignmentX(Component.CENTER_ALIGNMENT);
this.type.setAlignmentX(Component.CENTER_ALIGNMENT);
this.value.setAlignmentX(Component.CENTER_ALIGNMENT);
//value.setEditable(false);
this.add(label);
this.add(type);
this.add(value);
this.setBackground(Color.LIGHT_GRAY);
this.addMouseListener(this);
//button.setBorder(null);
//outerPanel.add(panel);

}
public String returnType(){
String retStr=new String();
    if(pinLabel.startsWith("A")){  //look if this is analog type
switch(Serial.getAnalogType()[panelNumber]){
    case 1: retStr="N/A"; break;
    case 2: retStr="pH";break;
    case 3: retStr="CO2";break;
    case 4: retStr="soil";break;
}
}else{
        if(pinLabel.startsWith("D")){  //look if this is analog type
switch(Serial.getDigitalType()[panelNumber-4]){
    case 1: retStr="N/A"; break;
    case 2: retStr="DHT11";break;
    case 3: retStr="DHT22";break;
    case 4: retStr="EC";break;
    case 5: retStr="dist";break;
    case 6: retStr="soil";break;    
}
    }else {
       switch(Serial.getI2CType()[panelNumber-3]){
    case 1: retStr="N/A"; break;
    case 2: retStr="light";break;
    case 3: retStr="time";break;
      
} 
        
        
        }

}
    return retStr;
}

public void retype(){
type.setText(returnType());
}

public void decolor(){
this.setBackground(Color.LIGHT_GRAY);
this.setBorder(BorderFactory.createRaisedBevelBorder());
}

public void setValue(String val){
value.setText(val);
}


    public void mousePressed(MouseEvent e) {
       
    }
     
    public void mouseReleased(MouseEvent e) {
        
    }
     
    public void mouseEntered(MouseEvent e) {
       
    }
     
    public void mouseExited(MouseEvent e) {
      
    }
     
    public void mouseClicked(MouseEvent e) {
     SensApl.setSelectedTab(tabNumber);
     SensApl.recolorButtons();
     this.setBackground(Color.ORANGE);
     this.setBorder(BorderFactory.createLoweredBevelBorder());
    }  
}
