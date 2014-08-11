/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package sensapl;

import java.awt.Component;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import javax.swing.*;

/**
 *
 * @author Jointar Ganjar
 */
public class DHTPanel extends ClassicPanel {
 
 public Plot plot2=new Plot();
 JLabel value2=new JLabel(); 
 
 public DHTPanel(String name) {
 super(name);
 
 //value2.setMaximumSize(value2.getPreferredSize());//setMaximumSize(new Dimension(value.getPreferredSize().width,20));
 value2.setAlignmentX(Component.CENTER_ALIGNMENT);
 this.add(this.value2);
 this.add(this.plot2);

    }
    
 @Override
  public void setValue(String val){
      String validVal;
    Pattern p = Pattern.compile("-?[\\d\\.]+");
    Matcher m = p.matcher(val);
     if (m.find()){
     validVal=m.group();
         System.out.println("number find> "+validVal);
         if(!validVal.equals("0")){
           int afterDecimal=validVal.length()-validVal.indexOf(".")-1; //counting number of digits after decimal point
           float newal=Float.parseFloat(validVal);
           this.value.setText(validVal+" °C");
           
           this.plot.addToGraph(newal, afterDecimal);
      m.find();
      validVal=m.group();
      this.value2.setText(validVal+" %");
      this.plot2.addToGraph(Integer.valueOf(validVal));
         }else this.value.setText(val);
     }
  /*  if (m.find()) {
  String validVal=m.group();
  if(validVal.contains(".")){
  int afterDecimal=validVal.length()-validVal.indexOf(".")-1; //counting number of digits after decimal point
  float newal=Float.parseFloat(validVal);
      for (int i = 0; i < afterDecimal; i++) {
          newal=newal*10;
          }
  this.plot.addToGraph((int)newal);
  }else this.plot.addToGraph(Integer.valueOf(validVal));
this.value.setText(validVal);
}else this.value.setText(val);  
     
     
     
     
     
  this.value.setText(val);
  plot.addToGraph(Integer.valueOf(val));*/
  }
    
}
