import java.io.*;
import java.net.*;
import java.util.*;

public class Client extends Thread {
    private String name;
    private List<String> preferences;
    private int preferenceIndex = 0;
    private volatile String partner = null;
    private volatile boolean running = true;
    private ServerSocket callbackSocket;
    private int callbackPort;
    private volatile long lastDumpTime = 0;

    private static final long STABILITY_TIMEOUT = 1000;

    public Client(String name, String[] preferenceList) {
        this.name = name;
        this.preferences = new ArrayList<>(Arrays.asList(preferenceList));
        
        try {
            this.callbackSocket = new ServerSocket(0);
            this.callbackPort = callbackSocket.getLocalPort();
            System.out.println(name + " callback server started on port " + callbackPort);
        } catch (IOException e) {
            System.out.println("Error creating callback socket: " + e.getMessage());
        }
    }

    @Override
    public void run() {
        new Thread(this::listenForDumpNotifications).start();
        
        while (running && preferenceIndex < preferences.size()) {
            if (partner == null) {
                proposeToNextPreference();
            } else {
                long stableTime = System.currentTimeMillis() - lastDumpTime;
                
                if (stableTime > STABILITY_TIMEOUT) {
                    System.out.println(name + " has a stable match with " + partner);
                    break;  
                }

                try {
                    Thread.sleep(100);
                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                    break;
                }
            }
        }
    }
    
    private void listenForDumpNotifications() {
        while (running) {
            try {
                Socket socket = callbackSocket.accept();
                BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
                String message = in.readLine();
                String[] parts = message.split(",");
                
                if (parts[0].equals("Dumped")) {
                    String womanName = parts[1];
                    System.out.println(name + " was dumped by " + womanName);
                    partner = null;
                    lastDumpTime = System.currentTimeMillis();
                }
                
                socket.close();
            } catch (IOException e) {
                if (!callbackSocket.isClosed()) {
                    System.out.println("Error in callback server: " + e.getMessage());
                }
            }
        }
    }

    private void proposeToNextPreference() {
        if (preferenceIndex >= preferences.size()) {
            return;
        }
        
        String womanName = preferences.get(preferenceIndex);
        int port = 8000 + Integer.parseInt(womanName.substring(5));
        
        try (Socket socket = new Socket("localhost", port);
             PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
             BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()))) {
            
            System.out.println(name + " proposing to " + womanName);
            out.println(name + "," + callbackPort);
            
            String response = in.readLine();
            if ("Accepted".equals(response)) {
                System.out.println(name + " was accepted by " + womanName);
                partner = womanName;
                lastDumpTime = System.currentTimeMillis();
            } else {
                System.out.println(name + " was rejected by " + womanName);
                preferenceIndex++;
            }
        } catch (IOException e) {
            System.out.println("Error proposing: " + e.getMessage());
            preferenceIndex++;
        }
    }
    
    public void stopClient() {
        running = false;
        try {
            if (callbackSocket != null && !callbackSocket.isClosed()) {
                callbackSocket.close();
            }
        } catch (IOException e) {
            System.out.println("Error closing callback socket: " + e.getMessage());
        }
    }

    public String getManName() {
        return name;
    }

    public String getPartner() {
        return partner;
    }
}