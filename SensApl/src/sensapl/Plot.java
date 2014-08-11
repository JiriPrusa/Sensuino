/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package sensapl;

import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.RenderingHints;
import java.util.ArrayList;
import javax.swing.JPanel;

/**
 *
 * @author Jointar Ganjar
 */
public class Plot extends JPanel {
private ArrayList<Integer> dataArrList = new ArrayList<Integer>();
int[] data=new int[dataArrList.size()]; 

    final int PAD = 20;  
   
protected void paintComponent(Graphics g) {  
          super.paintComponent(g);  
        
        Graphics2D g2 = (Graphics2D)g;  
        g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING,  
                            RenderingHints.VALUE_ANTIALIAS_ON);  
        if(data.length>=2){
        int w = getWidth();  
        int h = getHeight();  
        System.out.println("width "+ w);
        g2.drawLine(PAD, PAD, PAD, h-PAD);  
        g2.drawLine(PAD, h-PAD, w-PAD, h-PAD);  
        double xScale = (double)(w - 2*PAD)/(double)(data.length-1);  
        double maxValue=minmaxValue(data)[1];  
        double minValue=minmaxValue(data)[0];
            System.out.println("min a max>"+minValue+" "+maxValue);
        double yScale = (h - 2*PAD)/(maxValue-minValue); 
            System.out.println("Yscale "+yScale);
        // The origin location.  
      
        
        int x0 = PAD;  
        int y0 = h-PAD+(int)(yScale*minValue);  
        g2.setPaint(Color.red);  
        for(int j = 0; j < data.length-1; j++) {  
            int x1 = x0 + (int)(xScale * (j));  
            int y1 = y0 - (int)(yScale * data[j]);
            int x2 = x0 + (int)(xScale * (j+1));  
            int y2 = y0 - (int)(yScale * data[j+1]);
            g2.drawLine(x1, y1, x2, y2); 
           
        } 
      
        }
        
        /*
        int x0 = PAD;  
        int y0 = h-PAD;  
        g2.setPaint(Color.red);  
        for(int j = 0; j < data.length-1; j++) {  
            int x1 = x0 + (int)(xScale * (j+1));  
            int y1 = y0 - (int)(yScale * data[j]);
            int x2 = x0 + (int)(xScale * (j+2));  
            int y2 = y0 - (int)(yScale * data[j+1]);
            g2.drawLine(x1, y1, x2, y2);  
        } 
        */
        System.out.println("data length> "+ data.length);
    } 

public  void addToGraph(int in){
    System.out.println("adding int to graph...");
    
if((getWidth()-20)==dataArrList.size()){ 
dataArrList.remove(0);
    }


dataArrList.add(in);
data=new int[dataArrList.size()];
    for (int i = 0; i < data.length; i++) {
        data[i] = dataArrList.get(i);
        }
repaint();

}

public  void addToGraph(float in, int length){
    System.out.println("adding float to graph...");
 
    
      for (int i = 0; i < length; i++) {
          in=in*10;
          }
     
if((getWidth()-20)==dataArrList.size()){ 
dataArrList.remove(0);
    }


dataArrList.add((int)in);
data=new int[dataArrList.size()];
    for (int i = 0; i < data.length; i++) {
        data[i] = dataArrList.get(i);
        }
repaint();

}


private int[] minmaxValue(int[] ints) {
	int minmax[] = {ints[0],ints[0]};
	for (int i = 0; i < ints.length; i++) {
		if (ints[i] > minmax[1]) minmax[1] = ints[i];
		if (ints[i] < minmax[0]) minmax[0] = ints[i];
	}
	return minmax;
}
  
   }