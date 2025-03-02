import java.util.ArrayList;
import java.util.List;

public class App {
    private static final int N = 3;

    public static void main(String[] args) {
        String[][] menPreferences = {
            {"Woman2", "Woman1", "Woman3"},
            {"Woman2", "Woman3", "Woman1"},
            {"Woman3", "Woman2", "Woman1"}
        };
        
        String[][] womenPreferences = {
            {"Man2", "Man3", "Man1"},
            {"Man3", "Man1", "Man2"},
            {"Man2", "Man3", "Man1"},
        };
        
        List<Server> women = new ArrayList<>();
        for (int i = 0; i < N; i++) {
            String womanName = "Woman" + (i + 1);
            int port = 8000 + i + 1;
            Server woman = new Server(womanName, womenPreferences[i], port);
            women.add(woman);
            woman.start();
        }
        
        try {
            Thread.sleep(1000);
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }
        
        List<Client> men = new ArrayList<>();
        for (int i = 0; i < N; i++) {
            Client man = new Client("Man" + (i + 1), menPreferences[i]);
            men.add(man);
            man.start();
        }
        
        for (Client man : men) {
            try {
                man.join();
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        }
        
        System.out.println("FINAL PAIRINGS:");
        for (Client man : men) {
            System.out.println(man.getManName() + " paired with " + man.getPartner());
        }
        
        for (Server woman : women) {
            woman.stopServer();
        }
        
        for (Client man : men) {
            man.stopClient();
        }
    }
}