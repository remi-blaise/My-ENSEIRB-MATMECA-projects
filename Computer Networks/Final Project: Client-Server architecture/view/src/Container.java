import javax.swing.*;
import javax.imageio.ImageIO;
import java.io.*;
import java.util.List;
import java.awt.*;
import java.awt.event.*;

public class Container extends JPanel //generic lightweight container extented
{
  private Data fishes;
  private Image background;

  public Container(Data fishesD){
    super();
    fishes = fishesD;
  }

  public void paintComponent(Graphics g){
    g.drawImage(background, 0, 0, this.getWidth(), this.getHeight(), this);
	    for (Fish fish : fishes.getFishes())
	       g.drawImage(fish.getImage(), (int) fish.getPosition().getX(), (int) fish.getPosition().getY(),(int) fish.getX(fish.getDimension()),(int) fish.getY(fish.getDimension()),this);
  }

  public void setBackground(Image img) {
    this.background = img;
  }

  public int getWidth(){
    return 1853;
  }

  public int getHeight(){
    return 1000;
  }
}
