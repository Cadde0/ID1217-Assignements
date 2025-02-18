import java.util.LinkedList;
import java.util.Queue;

class Monitor{
    int menInside, womenInside = 0;
    Queue<Thread> queue = new LinkedList<Thread>();
    
    synchronized public void manEnter(Man man) throws InterruptedException{
        queue.add(man);
        Thread.sleep((int) Math.random() * 30000);
        while (womenInside > 0) {
            wait();
        }
        System.out.println("Man " + man.getId() + " enters");
        menInside++;
    }
    synchronized public void manExit(Man man) throws InterruptedException{
        menInside--;
        System.out.println("Man " + man.getId() + " exits");
        if(menInside == 0){
            notifyAll();
        }
    }
    synchronized public void womanEnter(Woman woman) throws InterruptedException{
        Thread.sleep((int) Math.random() * 30000);
        queue.add(woman);
        while (menInside > 0) {
            wait();
        }
        System.out.println("Woman " + woman.getId() + " enters");
        womenInside++;
    }
    synchronized public void womanExit(Woman woman) throws InterruptedException{
        womenInside--;
        System.out.println("Woman " + woman.getId() + " exits");
        if(womenInside == 0){
            notifyAll();
        }
    }
}

class Man extends Thread{
    Monitor monitor;
    
    public Man(Monitor monitor){
        this.monitor = monitor;
    }

    public void run(){
        try {
            monitor.manEnter(this);
            Thread.sleep((int) Math.random() * 30000);
            monitor.manExit(this);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}

class Woman extends Thread{
    Monitor monitor;
    
    public Woman(Monitor monitor){
        this.monitor = monitor;
    }

    public void run(){
        try {
            monitor.womanEnter(this);
            Thread.sleep((int) Math.random() * 30000);
            monitor.womanExit(this);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}

public class App {
    static int numOfPeople = 10;
    public static void main(String[] args) throws Exception {
        Monitor monitor = new Monitor();
        
        Thread[] people = new Thread[numOfPeople];
        
        for (int i = 0; i < people.length; i++) {
            if((i % 2) == 0){
                people[i] = new Man(monitor);
                continue;
            }
            people[i] = new Woman(monitor);
        }

        for (Thread thread : people) {
            thread.start();
        }
    }
}
