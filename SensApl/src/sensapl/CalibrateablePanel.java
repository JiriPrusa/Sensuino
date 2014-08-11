/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package sensapl;

import javax.swing.*;
import java.util.regex.Pattern;
import java.util.regex.Matcher;
/**
 *
 * @author Jirka
 */
public class CalibrateablePanel extends Panel{

    CalibrationBar calib;
    
public CalibrateablePanel(String name, int pin){
super(name);
calib=new CalibrationBar(name, pin);
this.add(calib);
this.add(plot);
}    
  
public void setCalibrationConstants(){
calib.repaintText();
}


}
