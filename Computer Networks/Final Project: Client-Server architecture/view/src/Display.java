import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import java.io.*;
import java.net.*;
import javax.imageio.ImageIO;
import java.util.Properties;
import javax.swing.Timer;

public class Display extends JFrame implements ActionListener {
  private Client client;
  private Container container;
  private Properties configFile;
  private Data fishes;
  private Parser parser;
  public JOptionPane jop ;
  private JTextField text;
  private JPanel p;


  public Display() throws IOException {
    configFile = new java.util.Properties();
    try {
      configFile.load(this.getClass().getClassLoader().getResourceAsStream("./affichage.cfg"));
    } catch (Exception e) {
      System.out.println("Problem loading ConfigFile...");
    }

    fishes = new Data();
    container = new Container(fishes);
    parser = new Parser();
    client = new Client(configFile,parser,fishes,container);
    jop = new JOptionPane();

    Dimension size=showBackground();
    showCommandLine(size);
    int delay = 30; // do a repaint each 30 miliseconds
    ActionListener taskPerformer = new ActionListener (){
    public void actionPerformed(ActionEvent arg0) {
      container.repaint();
      }
    };
    new Timer(delay,taskPerformer).start();
  }


  private Dimension showBackground(){
    try {
      Image img = ImageIO.read(new File("./ocean.jpg"));
      container.setBackground(img);
    } catch (IOException e) {
      System.out.println("Problem printing out the background picture.");
    }
    this.setTitle("Aquarium");
    Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
    this.setSize(screenSize.width, screenSize.height);
    this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    this.setLocationRelativeTo(null);
    getContentPane().add(container, "Center");
    this.setExtendedState(JFrame.MAXIMIZED_BOTH);
    return screenSize;
  }

  private void showCommandLine(Dimension screenSize){
    text = new JTextField("Enter your command ...", 15);
    this.p = new JPanel(new GridLayout());
    addBotton("Exit", p);
    getContentPane().add(p, "North");
    p.add(text);
    addBotton("OK", p);
    getContentPane().setPreferredSize(screenSize);
    this.pack();
    this.setVisible(true);
  }

  //add a botton with the label to P
  private void addBotton(String label, JPanel p) {
    JButton b = new JButton(label);
    p.add(b);
    b.addActionListener(this);
  }

  private void quit() {
    System.exit(0);
  }

  public void actionPerformed(ActionEvent e)
  {
    String cmd = e.getActionCommand();
    if (cmd.equals("Exit")) quit();
      else if (cmd.equals("OK")) {
        this.parser.parse(jop,text.getText(),client,fishes,container,configFile);
      }
  }

    public static void main(String[] args) throws Exception {
        Display disp = new Display();
    }
}
