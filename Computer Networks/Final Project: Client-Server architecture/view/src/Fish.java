import java.awt.*;
import java.util.List;
import javax.swing.Timer;
import java.awt.event.*;

public class Fish {
  private int id;
  private Point currentPosition;
  private final String name;
  private Point dimension;
  private List<Point> destinations;
  private boolean started;
  private Image img;
  private Timer timer;
  private int travelTime;

    public Fish(String name, int x, int y, int xdim, int ydim, List<Point> destinations, int time, Image img) {
    this.name = name;
    this.currentPosition = new Point(x, y);
    this.dimension = new Point(xdim, ydim);
    this.destinations = destinations;
    this.travelTime = time;
    verifyTraveltime(time);
    this.img = img;
    this.started = false;
  }

  public void setTravelTime(int t){
    this.travelTime = t;
  }

  public void verifyTraveltime(int time){
    if (time == 0) { //case where the time value is zero , show the fish directely at the destination
      Point pos = lastDestination();
      setPosition(getX(pos),getY(pos));
      deleteDestinations();
    }
  }

  public int destinationsSize(){
    return destinations.size();
  }

  public Point lastDestination(){
    int index = destinationsSize() -1;
    Point pos = destinations.get(index);
    return pos ;
  }

  public void setPosition(double x,double y){
    currentPosition.setLocation(x,y);
  }


  public void setDimension(double x,double y){
    dimension.setLocation(x,y);
  }

  public double getX(Point p){
    return p.getX();
  }

  public double getY(Point p){
    return p.getY();
  }

  public Point getDestination(int index){
    return destinations.get(index);
  }

  public void deleteDestinations(){
    destinations.clear();
  }

  public boolean noDestination(){
    return destinations.isEmpty();
  }

  public int getTravelTime(){
    return this.travelTime;
  }

  public void move(int v) {
    double theta = Math.atan2( destinations.get(0).getY() - currentPosition.getY(), destinations.get(0).getX() - currentPosition.getX());
    double distance = Math.hypot(currentPosition.getY() -  destinations.get(0).getY(), destinations.get(0).getX() - currentPosition.getX());

    double projectionX = currentPosition.getX() + v * Math.cos(theta);
    double projectionY = currentPosition.getY() + (v * Math.sin(theta));

    currentPosition.setLocation(projectionX, projectionY);
    if (currentPosition.equals(destinations.get(0))) {
      destinations.remove(0);
      if (destinations.isEmpty())
        stop();
    }
  }

  public void reachedGoal(){
    if(noDestination()){
      stop();
    }
  }

  public void start(int t) {
    int v = 1;
    double distance = 0.0;
    if (!destinations.isEmpty()) {
      distance = Math.hypot(destinations.get(0).getY() - currentPosition.getY(), destinations.get(0).getX() - currentPosition.getX());
      for (int i = 1; i < destinations.size(); i++) {
        distance += Math.hypot(destinations.get(i).getY() - destinations.get(i - 1).getY(), destinations.get(i).getX() - destinations.get(i - 1).getX());
      }
    }
    this.timer = new Timer(t * 1000 / (int) distance * v, new ActionListener() {
      public void actionPerformed(ActionEvent arg0) {
        move(v);
      }
    });
    this.timer.start();
  }

  public void updateDestinations(){
    if(currentPosition.equals(getDestination(0))){
      destinations.remove(0);
    }
  }

  public boolean isStarted() {
    return this.started;
  }

  public void fishStarted(){
    this.started = true;
  }

  public void stop() {
    this.timer.stop();
  }

  public Image getImage() {
    return this.img;
  }

  public void addDestination(int x, int y) {
    this.destinations.add(new Point(x, y));
  }

  public void addDimension(int x,int y){
    this.dimension = new Point(x,y);
  }

  public Point getPosition() {
    return currentPosition;
  }

  public Point getDimension() {
    return dimension;
  }

  public String getName() {
    return this.name;
  }
}
