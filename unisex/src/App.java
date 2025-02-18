public class App {
    class Man extends Thread{
        public synchronized void manEnter(){

        }
        public synchronized void manExit(){

        }   
    }
    class Woman extends Thread{
        public synchronized void womanEnter(){

        }
        public synchronized void womanExit(){

        }   
    }
    public static void main(String[] args) throws Exception {
        System.out.println("Hello, World!");
    }
}
