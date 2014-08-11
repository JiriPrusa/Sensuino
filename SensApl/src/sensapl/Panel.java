/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package sensapl;

import java.awt.Component;
import java.awt.Dimension;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import javax.swing.*;
/**
 *
 * @author Jointar Ganjar
 */
abstract class Panel extends JPanel {
 JLabel name=new JLabel();
 JLabel value=new JLabel("     ");
 public Plot plot=new Plot();
 
 public Panel(String name){
 this.setLayout(new BoxLayout(this, BoxLayout.Y_AXIS) );
 this.name.setText(name);
 this.name.setAlignmentX(Component.CENTER_ALIGNMENT);
 

 //value.setMaximumSize(value.getPreferredSize());//setMaximumSize(new Dimension(value.getPreferredSize().width,20));
 value.setAlignmentX(Component.CENTER_ALIGNMENT);
 
 this.add(this.name);
 this.add(this.value);
   
}

  public void setValue(String val){
  
    Pattern p = Pattern.compile("-?[\\d\\.]+");
    Matcher m = p.matcher(val);
    if (m.find()) {
  String validVal=m.group();
  if(validVal.contains(".")){
  int afterDecimal=validVal.length()-validVal.indexOf(".")-1; //counting number of digits after decimal point
  float newal=Float.parseFloat(validVal);
    
  this.plot.addToGraph(newal, afterDecimal);
  }else this.plot.addToGraph(Integer.valueOf(validVal));
this.value.setText(validVal);
}else this.value.setText(val);
    
    
  //this.value.setText(val);
  //plot.addToGraph(Integer.valueOf(val));
  }

}
