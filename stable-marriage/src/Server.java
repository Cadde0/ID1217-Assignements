import java.io.*;
import java.net.*;
import java.util.*;

public class Server extends Thread {
    private String name;
    private Map<String, Integer> preferences;
    private ServerSocket serverSocket;
    private String currentPartner = null;
    private int currentRank = Integer.MAX_VALUE;
    private int port;
    
    private static Map<String, Integer> clientCallbackPorts = new HashMap<>();

    public Server(String name, String[] preferenceList, int port) {
        this.name = name;
        this.preferences = new HashMap<>();
        for (int i = 0; i < preferenceList.length; i++) {
            preferences.put(preferenceList[i], i);
        }
        this.port = port;
    }

    public static void registerClientPort(String clientName, int callbackPort) {
        clientCallbackPorts.put(clientName, callbackPort);
    }

    @Override
    public void run() {
        try {
            serverSocket = new ServerSocket(port);
            System.out.println(name + " is waiting for proposals on port " + port);
            
            while (!serverSocket.isClosed()) {
                try {
                    Socket clientSocket = serverSocket.accept();
                    new ProposalHandler(clientSocket).start();
                } catch (SocketException e) {
                    break;
                }
            }
        } catch (IOException e) {
            System.out.println(name + " server error: " + e.getMessage());
        }
    }

    private class ProposalHandler extends Thread {
        private Socket socket;
        
        public ProposalHandler(Socket socket) {
            this.socket = socket;
        }
        
        @Override
        public void run() {
            try {
                BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
                PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
                
                String proposalStr = in.readLine();
                String[] parts = proposalStr.split(",");
                String manName = parts[0];
                int callbackPort = Integer.parseInt(parts[1]);
                
                clientCallbackPorts.put(manName, callbackPort);
                
                int rank = preferences.getOrDefault(manName, Integer.MAX_VALUE);
                
                synchronized (Server.this) {
                    if (currentPartner == null) {
                        currentPartner = manName;
                        currentRank = rank;
                        out.println("Accepted");
                        System.out.println(name + " accepted first proposal from " + manName);
                    } else if (rank < currentRank) {
                        String previousPartner = currentPartner;
                        currentPartner = manName;
                        currentRank = rank;
                        out.println("Accepted");
                        System.out.println(name + " dumped " + previousPartner + " for " + manName);
                        
                        notifyDumpedMan(previousPartner);
                    } else {
                        out.println("Rejected");
                        System.out.println(name + " rejected proposal from " + manName);
                    }
                }
                
                socket.close();
            } catch (IOException e) {
                System.out.println("Error handling proposal: " + e.getMessage());
            }
        }
        
        private void notifyDumpedMan(String manName) {
            Integer callbackPort = clientCallbackPorts.get(manName);
            if (callbackPort == null) {
                System.out.println("Warning: No callback port for " + manName);
                return;
            }
            
            try (Socket socket = new Socket("localhost", callbackPort);
                 PrintWriter out = new PrintWriter(socket.getOutputStream(), true)) {
                out.println("Dumped," + name);
                System.out.println(name + " sent dump notification to " + manName);
            } catch (IOException e) {
                System.out.println("Error notifying dumped man: " + e.getMessage());
            }
        }
    }

    public synchronized void stopServer() {
        try {
            serverSocket.close();
            System.out.println(name + " server stopped");
        } catch (IOException e) {
            System.out.println("Error stopping server: " + e.getMessage());
        }
    }
}