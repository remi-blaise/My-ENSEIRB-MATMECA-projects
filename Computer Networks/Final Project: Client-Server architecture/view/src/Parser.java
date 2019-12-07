import javax.swing.*;
import java.io.*;
import java.util.Properties;

public class Parser {
  String t ="5"; // default time

  private void statusFct(JOptionPane jop,Client client,Data data){
    boolean isConnected = client.protocol.socket.isConnected();
    int nbFish = data.fishesSize();
    String informationFishes = "";

    if(isConnected){
      client.protocol.send("status");
      System.out.println("Ok : Connecté au controleur, " + nbFish + " poissons trouvés.\n");
      for (Fish f : data.getFishes()){
        boolean started = f.isStarted();

        if(started){
          String currentFish =  "Fish " + f.getName() + " at " +  f.getPosition().getX() + "x" + f.getPosition().getY() + "," + f.getX(f.getDimension()) + "x" + f.getDimension().getY()/10 + " started \n";
          System.out.println(currentFish);
          informationFishes = informationFishes + "\n" + currentFish;
        }else{
          String currentFish =  "Fish " + f.getName() + " at " +  f.getPosition().getX() + "x" + f.getPosition().getY() + "," + f.getDimension().getX() + "x" + f.getDimension().getY()/10 + " not started \n";
          System.out.println(currentFish);
          informationFishes = informationFishes + "\n" + currentFish;
        }
      }
      jop.showMessageDialog(null, "OK : Connecté au controleur , "+ nbFish + " poissons trouvés\n" + informationFishes, "Response", JOptionPane.INFORMATION_MESSAGE);
    }else{
      client.protocol.send("status");
      System.out.println("NOk : Non connecté au controleur !!");
      jop.showMessageDialog(null, "NOK : Non connecté au controleur !!", "Response", JOptionPane.ERROR_MESSAGE);
    }
  }

  private void addFishFct(JOptionPane jop,String cmd,Container container,Properties configFile,Client client,Data data){
    String[] cmdT = cmd.trim().split(" ");
    int x = Integer.valueOf((cmdT[3].substring(0, cmdT[3].length() - 1).split("x"))[0]);
    int y = Integer.valueOf((cmdT[3].substring(0, cmdT[3].length() - 1).split("x"))[1]);
    int l = Integer.valueOf((cmdT[4].substring(0, cmdT[4].length() - 1).split("x"))[0]);
    int h = Integer.valueOf((cmdT[4].substring(0, cmdT[4].length() - 1).split("x"))[1]);
    String method = cmdT[5];
    int time = Integer.valueOf(t);
    boolean wasAdded = data.addFish(cmdT[1], x, y, l, h,t,container,configFile);
    if (wasAdded & method.equals("RandomWayPoint")) {
      client.protocol.send(cmd);
      System.out.println("Ok, fish added to the aquarium");
      jop.showMessageDialog(null, "OK, fish added to the aquarium", "Response", JOptionPane.INFORMATION_MESSAGE);
    }else{
      jop.showMessageDialog(null, "NOK : modèle de mobilité non supporté", "Response", JOptionPane.INFORMATION_MESSAGE);
    }
  }



  private void deleteFishFct(JOptionPane jop,String cmd,Data data){
    String[] cmdT = cmd.trim().split(" ");
    String fishD = cmdT[1];

    // see if it exists
    for (Fish f : data.getFishes()) {
      if (fishD.equals(f.getName())){
        if (data.removeFish(f)) {
          System.out.println("Ok .");
          jop.showMessageDialog(null, "OK.", "Response", JOptionPane.INFORMATION_MESSAGE);
        }else{
          System.out.println("NOK : Problem removing the fish");
          jop.showMessageDialog(null, "OK : Problem removing the fish", "Response", JOptionPane.INFORMATION_MESSAGE);
        }
      }
      else
      {
        System.out.println("NOK : Poisson Inexistant");
        jop.showMessageDialog(null, "OK : Poisson Inexistant", "Response", JOptionPane.INFORMATION_MESSAGE);
      }
    }
  }

  private void startFishFct(JOptionPane jop,String cmd,Data data){
    String[] cmdT = cmd.trim().split(" ");
    String fishD = cmdT[1];

    // see if it exists
    for (Fish f : data.getFishes()) {
      if (fishD.equals(f.getName())){
        f.start(5);
        f.fishStarted();
        System.out.println("Ok .");
        jop.showMessageDialog(null, "OK.", "Response", JOptionPane.INFORMATION_MESSAGE);
      }
    }
  }

  public void requestFishes(String reply,Data fishes,Container container,Properties configFile){
    int exists =0;
    String name,time;
    String[] dests,dim;
    String[] c = reply.split("list")[1].split("]");

    for(int i=0;i<c.length -1;i++){
      String[] fish = c[i].trim().split("\\[")[1].split(" ");
      name = fish[0];
      dests = fish[2].split(",")[0].split("x");
      dim = fish[2].split(",")[1].split("x");
      time = fish[2].split(",")[2];
      fishes.addFish(name,Integer.parseInt(dests[0]) , Integer.parseInt(dests[1]), Integer.parseInt(dim[0]), Integer.parseInt(dim[1]),time, container, configFile);
    }
  }


  void parse(JOptionPane jop,String cmd,Client client,Data data,Container container,Properties configFile) {
    String cmdT = cmd.trim(); //copy of the string, with leading and trailing whitespace omitted
    String answer;
    if (cmdT.equals("status")) {
      try {
        statusFct(jop,client,data);
      } catch (Exception ex) {
        System.out.print("error in status.");
      }
    } else if (cmdT.startsWith("addFish")) {
      addFishFct(jop,cmd,container,configFile,client,data);
    } else if (cmdT.startsWith("delFish")) {
      deleteFishFct(jop,cmd,data);
    } else if (cmdT.startsWith("startFish")){
      startFishFct(jop,cmd,data);
    } else if (cmdT.startsWith("getFishes")){
      try{
          client.protocol.getFishes(this,data,container);
          jop.showMessageDialog(null, "OK, View updated", "Response", JOptionPane.INFORMATION_MESSAGE);
      }catch(IOException ex){
          System.out.println("Problem in getFishes");
      }
    } else {
      jop.showMessageDialog(null, "NOK : commande introuvable", "Response", JOptionPane.INFORMATION_MESSAGE);
    }
  }
}
