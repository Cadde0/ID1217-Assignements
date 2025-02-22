import java.util.LinkedList;
import java.util.Queue;

public class App {
    private static final int NOF_THREADS = 1000;
    private static Monitor monitor = new Monitor();

    static class Monitor {
        private int menInBathroom = 0;
        private int womenInBathroom = 0;
        private Queue<Thread> queue = new LinkedList<>();

        public synchronized void manEnter(int id) throws InterruptedException {
            queue.add(Thread.currentThread());
            while (queue.peek() != Thread.currentThread() || womenInBathroom > 0) {
                wait();
            }
            queue.poll();
            menInBathroom++;
            System.out.println("Man " + id + " entered the bathroom. Men in bathroom: " + menInBathroom);
        }

        public synchronized void manExit(int id) {
            menInBathroom--;
            System.out.println("Man " + id + " left the bathroom. Men in bathroom: " + menInBathroom);
            if (menInBathroom == 0) {
                notifyAll();
            }
        }

        public synchronized void womanEnter(int id) throws InterruptedException {
            queue.add(Thread.currentThread());
            while (queue.peek() != Thread.currentThread() || menInBathroom > 0) {
                wait();
            }
            queue.poll();
            womenInBathroom++;
            System.out.println("Woman " + id + " entered the bathroom. Women in bathroom: " + womenInBathroom);
        }

        public synchronized void womanExit(int id) {
            womenInBathroom--;
            System.out.println("Woman " + id + " left the bathroom. Women in bathroom: " + womenInBathroom);
            if (womenInBathroom == 0) {
                notifyAll();
            }
        }
    }

    class Man extends Thread {
        private int id;

        public Man(int id) {
            this.id = id;
        }

        public void run() {
            try {
                while (true) {
                    work();
                    monitor.manEnter(id);
                    useBathroom();
                    monitor.manExit(id);
                }
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        private void work() throws InterruptedException {
            Thread.sleep((int) (Math.random() * 5000) + 3000);
        }

        private void useBathroom() throws InterruptedException {
            Thread.sleep((int) (Math.random() * 2000) + 1000);
        }
    }

    class Woman extends Thread {
        private int id;

        public Woman(int id) {
            this.id = id;
        }

        public void run() {
            try {
                while (true) {
                    work();
                    monitor.womanEnter(id);
                    useBathroom();
                    monitor.womanExit(id);
                }
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        private void work() throws InterruptedException {
            Thread.sleep((int) (Math.random() * 5000) + 3000);
        }

        private void useBathroom() throws InterruptedException {
            Thread.sleep((int) (Math.random() * 2000) + 1000);
        }
    }

    public static void main(String[] args) {
        App app = new App();
        for (int i = 0; i < NOF_THREADS; i++) {
            if (i % 2 == 0) {
                app.new Man(i).start();
            } else {
                app.new Woman(i).start();
            }
        }
    }
}
