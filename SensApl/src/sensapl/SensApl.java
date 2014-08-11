/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package sensapl;
import java.awt.*;
import javax.swing.*;
import java.awt.Component;
import java.awt.event.*;
import java.io.BufferedWriter;
import java.io.FileWriter;
import java.text.SimpleDateFormat;
import java.util.Date;
import javax.swing.border.Border;
import javax.swing.plaf.basic.BasicTabbedPaneUI;
/**
 *
 * @author Jirka
 */
public class SensApl extends JFrame implements ActionListener {

    static Serial mainSerial=new Serial();
    
    private static boolean changeCard;
    /**
     * Creates new form NewJFrame
     */
    public SensApl() {
        
        
        initComponents();
        this.pack();
        this.setSize(800,600);
        //mainSerial.initialize();
        //changePane();
    }

     public static boolean getChangeCard(){
       return changeCard;
       }
       
       public static void setChangeCard(boolean status){
       changeCard=status;
       }
    
    public static void setSelectedTab(int tabNumber){
    tabbedPane.setSelectedIndex(tabNumber);
    }
    
    public static void upgradePHsignal(int pin, float signal){
        switch (pin){
        case 0: panelA1.cardPH.calib.printActualSignal(signal);
        case 1: panelA2.cardPH.calib.printActualSignal(signal);
        case 2: panelA3.cardPH.calib.printActualSignal(signal);
         }
    }
    
     public static void upgradeECsignal(int pin, float signal){
        switch (pin){
        //case 0: panelD1.cardEC.calib.printActualSignal(signal); //digital pin 1 not possible when Serial
        case 1: panelD2.cardEC.calib.printActualSignal(signal);
        case 2: panelD3.cardEC.calib.printActualSignal(signal);
        case 3: panelD4.cardEC.calib.printActualSignal(signal);
         }
    }
    
    public static void recolorButtons(){
    A1Button.decolor();
    A2Button.decolor();
    A3Button.decolor();
    I2CButton.decolor();
    D1Button.decolor();
    D2Button.decolor();
    D3Button.decolor();
    D4Button.decolor();
    }
   
    public static void revalidateButtons(){
    A1Button.retype();
    A2Button.retype();
    A3Button.retype();
    I2CButton.retype();
    D1Button.retype();
    D2Button.retype();
    D3Button.retype();
    D4Button.retype();
    
    panelA1.untoggleButton(); 
    panelA2.untoggleButton(); 
    panelA3.untoggleButton(); 
    panelI2C.untoggleButton(); 
    panelD1.untoggleButton(); 
    panelD2.untoggleButton(); 
    panelD3.untoggleButton(); 
    panelD4.untoggleButton();
    }
    
    public static void revalidatePanels(){
    setChangeCard(true);
    panelA1.retype(); 
    panelA2.retype(); 
    panelA3.retype(); 
    panelI2C.retype(); 
    //panelD1.retype(); 
    panelD2.retype(); 
    panelD3.retype(); 
    panelD4.retype();
    setChangeCard(false);
    }
    
    public static void revalidateConstants(){
    panelA1.reconst(); 
    panelA2.reconst(); 
    panelA3.reconst(); 
    panelI2C.reconst(); 
    
    panelD2.reconst(); 
    panelD3.reconst(); 
    panelD4.reconst();
    }
     
     public static void revalidateValues(String D2val, String D3val, String D4val, String A1val, String A2val, String A3val, String I2Cval){
    System.out.println("start reval buttons....");
    A1Button.setValue(A1val);
    A2Button.setValue(A2val);
    A3Button.setValue(A3val);
    I2CButton.setValue(I2Cval);
    //D1Button.setValue();
    D2Button.setValue(D2val);
    D3Button.setValue(D3val);
    D4Button.setValue(D4val);
    
    panelA1.addValue(A1val);
    panelA2.addValue(A2val);
    panelA3.addValue(A3val);
    panelI2C.addValue(I2Cval);
    panelD2.addValue(D2val);
    panelD3.addValue(D3val);
    panelD4.addValue(D4val);
         System.out.println("revalidating values....");
         if(dataCollectButton.isSelected()){
         dataLog.logData(dateFormat.format(sensuinoTime), D2val, D3val, D4val);
         }
         
     }
    
   
                           
    private void initComponents() {
       //Insets insets = this.getInsets();
        
       portSelecter=new JComboBox(mainSerial.portEnum());
       portSelecter.addActionListener(this);
        
        connectButton=new JButton("connect");
        connectButton.addActionListener(this);
        
        disconnectButton=new JButton("disconnect");
        disconnectButton.addActionListener(this);
        
        connectStatus=new JLabel("DISCONNECTED");
        connectStatus.setForeground(Color.RED);
        
        topPanel=new JPanel();
        leftSidePanel = new JPanel();
        rightSidePanel = new JPanel();
        
        buttonPanel = new JPanel();
        tabbedPane = new JTabbedPane();
        
        tabbedPane.setUI(new BasicTabbedPaneUI() {  
    @Override  
    protected int calculateTabAreaHeight(int tab_placement, int run_count, int max_tab_height) {  
      
            return 0;  
    }  
     @Override  
    protected int calculateTabAreaWidth(int tab_placement, int run_count, int max_tab_height) {  
      
            return 0;  
    }  
    
}); 
        
        buttonPanelHolder=new JPanel();
        buttonPanelBackground=new JLabel();
        
        A1Button = new SensButton(0,"A1");
        A2Button = new SensButton(1,"A2");
        A3Button = new SensButton(2,"A3");
        I2CButton = new SensButton(3,"I2C");
        D1Button = new SensButton(4,"D1");
        D2Button = new SensButton(5,"D2");
        D3Button = new SensButton(6,"D3");
        D4Button = new SensButton(7,"D4");
        
        
        panelA1 = new AnalogTabPanel(0);
        panelA2 = new AnalogTabPanel(1);
        panelA3 = new AnalogTabPanel(2);
        panelI2C = new I2CTabPanel(0);
        panelD1 = new DigitalTabPanel(0);
        panelD2 = new DigitalTabPanel(1);
        panelD3 = new DigitalTabPanel(2);
        panelD4 = new DigitalTabPanel(3);
        
        dataCollectButton=new JToggleButton("Ukladani dat");
        System.out.println(dataCollectButton.getMaximumSize());
        dataCollectButton.setMaximumSize(new Dimension(240,55));
        dataCollectButton.setPreferredSize(new Dimension(240,55));
        dataCollectButton.setAlignmentX(Component.CENTER_ALIGNMENT);
        
        
        delay=new TextField("unknown");
        
        delaySetButton=new JButton("Nastav delay");
        delaySetButton.addActionListener(this);
        
        timeSetButton=new JButton("Nastav cas");
        timeSetButton.addActionListener(this);
        
        time=new TextField("unknown");
        time.addFocusListener(new FocusListener() {
            String timeBefore;
            @Override
            public void focusGained(FocusEvent e) {
                timeSelected=true;
                timeBefore=time.getText();
             
            }

            @Override
            public void focusLost(FocusEvent e) {
                 timeSelected=false; 
                 String timeAfter=time.getText();
                 if(!timeAfter.equals(timeBefore)){
                 timeEdited=true;
                 }else timeEdited=false;
            }
            });
        
        delayPanel=new JPanel();
        delayPanel.setLayout(new GridLayout(1,2));
        delayPanel.setBackground(Color.DARK_GRAY);
        delayPanel.setBorder(BorderFactory.createEmptyBorder(3,5,3,5));
        
        delayPanel.add(delay);
        delayPanel.add(delaySetButton);
                
        timePanel=new JPanel();
        timePanel.setLayout(new GridLayout(1,2));
        timePanel.setBackground(Color.DARK_GRAY);
        timePanel.setBorder(BorderFactory.createEmptyBorder(3,5,1,5));

        timePanel.add(time);
        timePanel.add(timeSetButton);

        setDefaultCloseOperation(javax.swing.WindowConstants.EXIT_ON_CLOSE);

        //SensButton.setText("jToggleButton1");

leftSidePanel.setLayout(new BoxLayout(leftSidePanel, BoxLayout.Y_AXIS));
leftSidePanel.setBackground(Color.DARK_GRAY);
leftSidePanel.add(buttonPanelHolder);
leftSidePanel.add(dataCollectButton);
 leftSidePanel.add(delayPanel);
leftSidePanel.add(timePanel);



A1Button.setBounds(2, 214, 110, 80);
A2Button.setBounds(2, 115, 110, 80);
A3Button.setBounds(2, 8, 110, 80);
I2CButton.setBounds(2, 315, 110, 80);

D4Button.setBounds(162, 8, 110, 80);
D3Button.setBounds(162, 115, 110, 80);
D2Button.setBounds(162, 214, 110, 80);
D1Button.setBounds(162, 315, 110, 80);

buttonPanel.setOpaque(false);
buttonPanel.setLayout(null);

buttonPanel.add(A1Button);
buttonPanel.add(A2Button);
buttonPanel.add(A3Button);
buttonPanel.add(I2CButton);

buttonPanel.add(D1Button);
buttonPanel.add(D2Button);
buttonPanel.add(D3Button);
buttonPanel.add(D4Button);



buttonPanelBackground.setIcon(new ImageIcon(getClass().getResource("/resources/background.png"))); 

buttonPanelHolder.setLayout(new GridBagLayout());

GridBagConstraints gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
buttonPanelHolder.add(buttonPanel, gridBagConstraints);
buttonPanelHolder.add(buttonPanelBackground, gridBagConstraints);        

        tabbedPane.setTabPlacement(JTabbedPane.RIGHT);

        tabbedPane.addTab("tab1", panelA1);

        tabbedPane.addTab("tab2", panelA2);

        tabbedPane.addTab("tab3", panelA3);

        tabbedPane.addTab("tab4", panelI2C);

        tabbedPane.addTab("tab5", panelD1);

        tabbedPane.addTab("tab6", panelD2);

        tabbedPane.addTab("tab7", panelD3);

        tabbedPane.addTab("tab8", panelD4);

        javax.swing.GroupLayout rightSidePanelLayout = new javax.swing.GroupLayout(rightSidePanel);
        //rightSidePanel.setLayout(rightSidePanelLayout);
        rightSidePanel.setBackground(Color.ORANGE);
       rightSidePanel.setLayout(new BorderLayout());
      
       Border ohr;
       ohr = BorderFactory.createEmptyBorder(10,10,10,10);
       
rightSidePanel.setBorder(ohr);
rightSidePanel.add(tabbedPane,BorderLayout.CENTER);
        
//getContentPane().setLayout(layout); 
 topPanel.setBackground(Color.black);
topPanel.add(connectStatus);
  topPanel.add(connectButton);
   topPanel.add(disconnectButton);
   topPanel.add(portSelecter);
        
 getContentPane().add(topPanel, BorderLayout.NORTH);
 getContentPane().add(leftSidePanel, BorderLayout.WEST);
 getContentPane().add(rightSidePanel, BorderLayout.CENTER);

        pack();
        
    }// </editor-fold>                        

    /**
     * @param args the command line arguments
     */
    public static void main(String args[]) {
        /* Set the Nimbus look and feel */
        //<editor-fold defaultstate="collapsed" desc=" Look and feel setting code (optional) ">
        /* If Nimbus (introduced in Java SE 6) is not available, stay with the default look and feel.
         * For details see http://download.oracle.com/javase/tutorial/uiswing/lookandfeel/plaf.html 
         */
        
        try {
            for (javax.swing.UIManager.LookAndFeelInfo info : javax.swing.UIManager.getInstalledLookAndFeels()) {
                if ("Nimbus".equals(info.getName())) {
                    javax.swing.UIManager.setLookAndFeel(info.getClassName());
                    break;
                }
            }
        } catch (ClassNotFoundException ex) {
            java.util.logging.Logger.getLogger(SensApl.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
        } catch (InstantiationException ex) {
            java.util.logging.Logger.getLogger(SensApl.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
        } catch (IllegalAccessException ex) {
            java.util.logging.Logger.getLogger(SensApl.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
        } catch (javax.swing.UnsupportedLookAndFeelException ex) {
            java.util.logging.Logger.getLogger(SensApl.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
        }
        //</editor-fold>

        /* Create and display the form */
        java.awt.EventQueue.invokeLater(new Runnable() {
            @Override
            public void run() {
                new SensApl().setVisible(true);
            }
        });
    }
    // Variables declaration - do not modify                     
    public static JComboBox portSelecter;
    
    private JButton connectButton;
    private JButton disconnectButton;
    
    private static JLabel connectStatus;
    
    private static DigitalTabPanel panelD1; //DigitalTabPanel
    private static DigitalTabPanel panelD2; //DigitalTabPanel
    private static DigitalTabPanel panelD3; //DigitalTabPanel
    private static DigitalTabPanel panelD4; //DigitalTabPanel
   
    private static AnalogTabPanel panelA1; //AnalogTabPanel 
    private static AnalogTabPanel panelA2; //AnalogTabPanel 
    private static AnalogTabPanel panelA3; //AnalogTabPanel 
    private static TabPanel panelI2C; //I2CTabPanel 
    
    private JPanel topPanel;
    
    private JPanel leftSidePanel;
    private JPanel rightSidePanel;
    
    private JPanel buttonPanelHolder;
    private JPanel buttonPanel;
    private JLabel buttonPanelBackground;
    
    private static JTabbedPane tabbedPane;
    private static SensButton A1Button;
    private static SensButton A2Button;
    private static SensButton I2CButton;
    private static SensButton A3Button;
    private static SensButton D1Button;
    private static SensButton D2Button;
    private static SensButton D3Button;
    private static SensButton D4Button;
    
    public static JPanel delayPanel;
    public static TextField delay;
    public static JButton delaySetButton;
    
    public static JPanel timePanel;
    public static TextField time;
    public static JButton timeSetButton;  
    
    public static JToggleButton dataCollectButton;
   
    public static boolean timeEdited;
    public static boolean timeSelected;
    
    public static Date sensuinoTime;
    public static SimpleDateFormat dateFormat=new SimpleDateFormat("hh:mm:ss dd.MM.yyyy");

    private static DataCollectThread dataLog=new DataCollectThread();
    // End of variables declaration 
    
    public static void setConectionLabel(boolean status){
    if(status){
    connectStatus.setText("CONNECTED");
    connectStatus.setForeground(Color.GREEN);
    }else{
    connectStatus.setText("DISCONNECTED");
    connectStatus.setForeground(Color.RED);
    }
    
    }
    
     public static void takeDelay(long delayTime){
     delay.setText(String.valueOf(delayTime));
   
 }
    
   public static void takeDate(long now){
   if(!timeSelected){
   sensuinoTime=new Date(now*1000L);
   
   time.setText(dateFormat.format(sensuinoTime));
   }
 }
    
    @Override
    public void actionPerformed(ActionEvent evt){
    if (evt.getSource()==connectButton){
    mainSerial.startComunication();
    }
    if (evt.getSource()==disconnectButton){
    mainSerial.stopComunication();
    setConectionLabel(false);
    }
    if (evt.getSource()==portSelecter){
        System.out.println(portSelecter.getSelectedItem());
    }
    
    if(evt.getSource()==timeSetButton){
        mainSerial.setTime();
        try{Thread.sleep(1000L);
        }catch(Exception ex){}
    }
    
     if(evt.getSource()==delaySetButton){
        mainSerial.setDelay();
        try{Thread.sleep(1000L);
        }catch(Exception ex){}
    }
    /* 
      if(e.getSource()==dataCollectButton){
        if (dataCollectButton.isSelected()) {
       new DataCollectThread().start();
           System.out.println("Start collecting data");
            } else {
         
         System.out.println("Stop collecting data");
            }
 }
*/    
    }
    
}

class DataCollectThread extends Thread {

     private String D1val;
     private String D2val;
     private String D3val;
     private String D4val;
    
 public void run() {                                
   
     try{
  FileWriter fstream = new FileWriter("Output.txt",true);
  BufferedWriter out = new BufferedWriter(fstream);
                 
    //fstream.write("time\t\t\ttemp1[°C]\ttemp2[°C]\ttemp3[°C]\thyg1[%]\thyg2[%]\thyg3[%]\tEC[ppm]\tpH[]\tWTemp[°C]\tCO2[ppm]\tTriak1\tTraik2\tTriak3\n");
    // fstream.write(Viewer.time.getText()+"\t"+Viewer.temp_1.getValue()+"\t\t"+Viewer.temp_2.getValue()+"\t\t"+Viewer.temp_3.getValue()+"\t\t"+Viewer.hyg_1.getValue()+"\t\t"+Viewer.hyg_2.getValue()+"\t\t"+Viewer.hyg_3.getValue()+"\t\t"+Viewer.ec.getValue()+"\t\t"+Viewer.ph.getValue()+"\t\t"+Viewer.temp_water.getValue()+"\t\t"+Viewer.co2.getValue()+"\t\t"+Viewer.timer1.getStatus()+"\t\t"+Viewer.timer2.getStatus()+"\t\t"+Viewer.timer3.getStatus()+"\n");
    fstream.write("pokus "+D1val+"  "+D2val+"  "+D3val+"\n");
    out.flush();
    //Thread.sleep(5000L);
    
    out.close();
         System.out.println("Data logged");
  //this.wait();
       }catch(Exception e){//Catch exception if any
  System.err.println("Error: " + e.getMessage());
  }
     
      }
 
 public void logData(String D1, String D2, String D3, String D4){
 this.D1val=D1;
 this.D2val=D2;
 this.D3val=D3;
 this.D4val=D4;
 this.run();
 }

}

