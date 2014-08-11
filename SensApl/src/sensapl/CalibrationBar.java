/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package sensapl;

import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.JToggleButton;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;

/**
 *
 * @author Jointar Ganjar
 */

public class CalibrationBar extends JPanel implements ActionListener{

Dimension screenSize=Toolkit.getDefaultToolkit().getScreenSize();
Dimension frameSize=new Dimension((int)(screenSize.width/2),(int)(screenSize.height/6));
    
private JLabel title=new JLabel();
public JToggleButton startCalib=new JToggleButton("START");
public JLabel equation=new JLabel();
public String x,y,a,b;
public float x1,x2,y1,y2;
public float eqA,eqB;
public CalibrationNode LOW,HIGH;
public JButton calcul=new JButton("Dopocitej");
public JTextField aValue=new JTextField("a=");
public JTextField bValue=new JTextField("b=");
public JButton sendButton=new JButton("SEND");
private JPanel controlPanel=new JPanel();
private JPanel titlePanel=new JPanel();
public int pin;
public String type;
//private boolean newConsCalculated;


public CalibrationBar(String name, int pin){
this.setSize(new Dimension(20,50));
this.setLayout(new GridLayout(1,4));
this.setAlignmentX(Component.CENTER_ALIGNMENT);
this.setMaximumSize(frameSize);
this.setBorder(BorderFactory.createEmptyBorder(30,10,30,10));
this.setBackground(Color.ORANGE);
this.pin=pin;
 
//newConsCalculated=false;

title.setText(name);
pin=(int)name.charAt(name.length()-1);
if(name.contains("EC"))type="EC";
else type="PH";
//aValue.setEditable(false);
//bValue.setEditable(false);
aValue.addMouseListener(new MouseAdapter()
    {
        public void mouseClicked(MouseEvent e)
        {
            if(aValue.getText().equals("a="))
            {
                aValue.setText("");
                repaint();
                revalidate();
            }           
        }
    });

bValue.addMouseListener(new MouseAdapter()
    {
        public void mouseClicked(MouseEvent e)
        {
            if(bValue.getText().equals("b="))
            {
                bValue.setText("");
                repaint();
                revalidate();
            }           
        }
    });

LOW=new CalibrationNode(this,"LOW");
HIGH=new CalibrationNode(this,"HIGH");

//equation.setText(y+"="+x+"/"+a+"+"+b);
repaintText();

startCalib.addActionListener(this);
calcul.addActionListener(this);
sendButton.addActionListener(this);

controlPanel.setLayout(new GridLayout(4,1));
controlPanel.add(calcul);
controlPanel.add(aValue);
controlPanel.add(bValue);
controlPanel.add(sendButton);
titlePanel.setLayout(new GridLayout(3,1));
title.setHorizontalAlignment(JLabel.CENTER);
titlePanel.add(title);
titlePanel.add(startCalib);
equation.setHorizontalAlignment(JLabel.CENTER);
titlePanel.add(equation);



this.add(titlePanel);
this.add(LOW);
this.add(HIGH);
this.add(controlPanel);



} 
public void repaintText(){
    if(type.equals("EC")){
        a=String.valueOf(SensApl.mainSerial.getECconst(pin, 0));
        b=String.valueOf(SensApl.mainSerial.getECconst(pin, 1));
    }else{
    a=String.valueOf(SensApl.mainSerial.getPHconst(pin, 0));
    b=String.valueOf(SensApl.mainSerial.getPHconst(pin, 1));
    }
    
equation.setText(y+"="+x+"/"+a+"+"+b);
}

public void printActualSignal(float signal){
if(!LOW.getValue.isSelected())LOW.setActualValue(String.valueOf(signal));
if(!HIGH.getValue.isSelected())HIGH.setActualValue(String.valueOf(signal));
}

public void actionPerformed (ActionEvent e){
 
    /*LOW button*/
        if(e.getSource()==LOW.getValue){
    if(LOW.getValue.isSelected()){
     x1=Float.parseFloat(LOW.actualValue.getText());
       boolean badFormat=false;
     try {
          y1=Float.parseFloat(LOW.standartValue.getText()); 
        } catch (NumberFormatException ex) {
          System.out.println("Bad input standart value format");
          badFormat=true;  
        }
        if(badFormat) LOW.getValue.setSelected(false);
        else LOW.standartValue.setEditable(false);
    
    }else LOW.standartValue.setEditable(true);
  }
    
   /*HIGH button*/
    if(e.getSource()==HIGH.getValue){
    if(HIGH.getValue.isSelected()){
    x2=Float.parseFloat(HIGH.actualValue.getText());
     boolean badFormat=false;
     try {
    y2=Float.parseFloat(HIGH.standartValue.getText());
    } catch (NumberFormatException ex) {
         System.out.println("Bad input standart value format");
          badFormat=true;  
        }
        if(badFormat)HIGH.getValue.setSelected(false);
        else HIGH.standartValue.setEditable(false);
        
    }else HIGH.standartValue.setEditable(true);  
  }
    
  /*calcul button*/  
  if(e.getSource()==calcul){
      if(LOW.getValue.isSelected()&&HIGH.getValue.isSelected()){
   eqA=(x2-x1)/(y2-y1);
   eqB=y2-x2/eqA;
   aValue.setText(Float.toString(eqA));
   bValue.setText(Float.toString(eqB));
   //newConsCalculated=true;
      }
  }
    
 /*for EC type bar*/   
    if(type.equals("EC")){
    if(e.getSource()==startCalib){
  if(startCalib.isSelected()){
    
      SensApl.mainSerial.getECpure(pin);
      /*
      Sensuino.main.getECcontstants(pin);
      */ 
      sendButton.setEnabled(false);
      System.out.println("Start calibrating EC"+pin);
      
  }else{
  SensApl.mainSerial.calibrationStop();
  sendButton.setEnabled(true);
  System.out.println("Stop calibrating");
  }    
  }
 
 if(e.getSource()==sendButton){
    if(isNumber(aValue.getText()) && isNumber(bValue.getText())){
        try{
        SensApl.mainSerial.sendECcontstants(pin, Float.parseFloat(aValue.getText()),Float.parseFloat(bValue.getText()));
        System.out.println("Sending EC constants"); 
        System.out.println("send ECa and ECb"+aValue.getText()+" "+bValue.getText()+" on pin "+pin);  
        }catch(Exception ex){
         System.out.println(ex.toString());
        }
    }
  }
   
 }else{
        
/*for pH type bar*/  
    if(e.getSource()==startCalib){
    if(startCalib.isSelected()){
     SensApl.mainSerial.getPHpure(pin);
        
        /*
      Sensuino.main.getPHcontstants(pin);
            
      */
      sendButton.setEnabled(false);
      System.out.println("Start calibrating PH"+pin);
      
  }else{
  SensApl.mainSerial.calibrationStop();
  
  sendButton.setEnabled(true);
  System.out.println("Stop calibrating");
  }    
  }
 
 if(e.getSource()==sendButton){
    if(isNumber(aValue.getText()) && isNumber(bValue.getText())){
        try{
        SensApl.mainSerial.sendPHcontstants(pin, Float.parseFloat(aValue.getText()),Float.parseFloat(bValue.getText()));
        System.out.println("Sending PH constants"); 
        System.out.println("send PHa and PHb "+aValue.getText()+" "+bValue.getText()+" on pin "+pin);    
        }catch(Exception ex){
         System.out.println(ex.toString());
            } 
    }    
  }
  
  }
 
 
 
}

private boolean isNumber (String in){
try{
Float.parseFloat(in);
}catch(Exception ex){
    System.out.println(in + " is not a valid number");
    return false;
}
return true;
} 

}
class CalibrationNode extends JPanel{
JLabel title=new JLabel();
JTextField standartValue=new JTextField("Hodnota standartu");
JTextField actualValue=new JTextField("aktualni hodnota");
JToggleButton getValue;
    

public CalibrationNode(CalibrationBar owner, String title){
this.setLayout(new GridLayout(4,1));

this.title.setText(title);
this.title.setHorizontalAlignment(JLabel.CENTER);
getValue=new JToggleButton("GET "+title);
getValue.addActionListener(owner);
standartValue.addMouseListener(new MouseAdapter()
    {
        public void mouseClicked(MouseEvent e)
        {
            if(standartValue.getText().equals("Hodnota standartu"))
            {
                standartValue.setText("");
                repaint();
                revalidate();
            }           
        }
    });

actualValue.setEditable(false);
this.add(this.title); 
this.add(standartValue);
this.add(actualValue);
this.add(getValue);
   
}

public void setActualValue(String value){
actualValue.setText(value);
}
    
    
}
