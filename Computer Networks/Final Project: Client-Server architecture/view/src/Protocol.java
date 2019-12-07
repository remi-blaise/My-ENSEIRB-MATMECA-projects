import java.net.*;
import java.util.Properties;
import java.net.Socket;
import java.io.*;

public class Protocol {
  protected Socket socket;
  protected Properties configFile; // a persistent set of properties saved to a stream or loaded from a stream.
  protected BufferedReader in; // reads text from a character-input stream
  protected PrintWriter out;  // Prints formatted representations of objects to a text-output stream
  protected String id;

  public Protocol(Properties config) {
    this.configFile = config;
    initSocket();
  }

  private void initSocket() {
    try {
      socket = new Socket(configFile.getProperty("controller-address"), Integer.valueOf(configFile.getProperty("controller-port")));
    } catch (ConnectException exception) {
      System.out.println("Server is down.");
      System.exit(1);
    } catch (UnknownHostException e) {
      System.out.println("Unknown host.");
      System.exit(1);
    } catch (IOException e) {
      System.out.println("Impossible to connect to server.");
      System.exit(1);
    }

    try {
      in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
    } catch (IOException e) {
      System.out.println("Impossible to read from server.");
    }

    try {
      out = new PrintWriter(socket.getOutputStream(), true);
    } catch (IOException e) {
      System.out.println("Impossible to write to server.");
    }
  }

  //send a message and recieve the answer
  public String request(String message) throws IOException {
    out.println(message);
    String answer;
    answer = in.readLine();
    out.flush();
    return answer;
  }

  public void send(String message) {
    out.println(message);
  }

  private void end() throws IOException {
    in.close();
    out.close();
    socket.close();
  }

  //  Returns ID recieved from the server after initialisation
  public String hello() throws IOException {
    String id = configFile.getProperty("id");
    String response = request("hello in as " + id  + "\n");
    System.out.println(response);
    if (response.split(" ")[0] == "no") {
      System.out.println("ID " + id + " not available and no free ID.");
      System.exit(1);
    } else if (response.split(" ")[0] == "greeting") {
      this.id=response.split(" ")[1];
      return response.split(" ")[1];
    }
      this.id=response.split(" ")[1];
      return response.split(" ")[1];
  }

  public void getFishes(Parser parser,Data fishes,Container container) throws IOException{
    String reply = request("getFishes");
    System.out.println(reply);
    parser.requestFishes(reply,fishes,container,configFile);
  }

  public void addFirstFish(){
    send("addFish PoissonRouge at 10x10, 4x3, RandomWayPoint");
  }

  public void ping(){
    try{
      request("ping " + configFile.getProperty("controller-port"));
    }
    catch(IOException ex){
      System.out.println("Problem in getFishes");
    }
  }
  public void logOut() throws IOException{
    send("log out");
    end();
  }
}
