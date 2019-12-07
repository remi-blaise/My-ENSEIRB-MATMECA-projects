import javax.imageio.ImageIO;
import java.awt.*;
import java.io.*;
import java.util.ArrayList;
import java.util.concurrent.*;
import java.util.Properties;


public class Data {
  private final CopyOnWriteArrayList<Fish> fishes;//A thread-safe variant of ArrayList in which all mutative operations (add, set, and so on) are implemented by making a fresh copy of the fishes array

  public Data(){
    fishes = new CopyOnWriteArrayList<Fish>();
  }

  public CopyOnWriteArrayList<Fish> getFishes(){
    return this.fishes;
  }

  public boolean addFish(String name, int x, int y, int length, int height,String time , Container container, Properties configFile) {
    int widthC = container.getWidth();// pourcentage of the screen size
    int heightC = container.getHeight();

     // see if the fish already exist
    for (int j=0;j<fishesSize();j++){
      if (name.equals(fishes.get(j).getName())){
          fishes.get(j).addDestination(x, y);
          fishes.get(j).setDimension(length,height);
          fishes.get(j).setTravelTime(Integer.valueOf(time));
      }
    }
    try {

      File f = File.createTempFile(name, ".png");
      Image img;
      if (f.exists()) {
        img = ImageIO.read(new File(configFile.getProperty("resources") + "/" + name.split("_")[0] + ".png"));
      }
      else
      {
        img = ImageIO.read(new File(configFile.getProperty("resources") + "/fish.png")); // default picture
      }

      ArrayList<Point> dests = new ArrayList<Point>();
      dests.add(new Point(200, 500));
      dests.add(new Point(100, 400));
      dests.add(new Point(100, 100));
      dests.add(new Point(200, 400));

      Fish fish = new Fish(name, widthC /100 * x, heightC /100 * y,widthC /100 * length, heightC /100 * height, dests, 5, img);
      fishes.add(fish);

    } catch (IOException e) {
      System.out.println("error adding fish");
    }
    return true;
  }

  public int fishesSize(){
    return fishes.size();
  }

  public boolean removeFish(Fish f){
    return fishes.remove(f);
  }

}
