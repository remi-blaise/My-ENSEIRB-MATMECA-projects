import java.lang.Thread;
import java.util.Properties;
import java.io.IOException;
import java.awt.event.*;
import javax.swing.Timer;


class Client {
  private String id; // client's id
  Protocol protocol;

  public Client(Properties configFile,Parser parser,Data fishes,Container container) throws IOException {
    Protocol protocol = new Protocol(configFile);
    this.protocol = protocol;

    //initialize the server/client comunication
    protocol.hello();
    this.id=protocol.id;

    new Timer(Integer.valueOf(configFile.getProperty("display-timeout-value")) * 1000, new ActionListener() { // send ping each 30 seconds
            public void actionPerformed(ActionEvent arg0) {
                protocol.ping();
            }
    }).start();


    //getFishes
    Thread thread = new Thread(){
      public void run() {
        while (true){
          try {
            protocol.getFishes(parser,fishes,container);
          } catch (IOException e) {

          }
          try {
            Thread.sleep(500000);
          } catch (Exception e) {
            System.out.println("Thread Interrupted !");
          }
        }
      }
    };
    thread.start();
  }
}
