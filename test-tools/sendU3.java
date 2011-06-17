
/**
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2002</p>
 * <p>Company: </p>
 * @author unascribed
 * @version 1.0
 */
import java.net.*;
import java.io.*;
public class sendU3 {
  public final static int DEFAULT_PORT=7;
  public static void main(String[] args){
    String hostname=args[0];
    int port=5060;
    if(args.length >0){
      hostname=args[0];
    }
    try{
      InetAddress ia=InetAddress.getByName(hostname);
      SenderThread sender=new SenderThread(ia,port);
      sender.start();
      ReceiverThread receiver=new ReceiverThread(sender.getSocket());
      receiver.start();
    }
    catch(UnknownHostException e){
      System.err.println(e);
    }
    catch(SocketException se){
      System.err.println(se);
    }
  }
  static class ReceiverThread extends Thread {
          DatagramSocket socket;
          private boolean stopped=false;
          public ReceiverThread(DatagramSocket ds)throws SocketException {
            this.socket =ds;
          }
          public void halt(){
            this.stopped=true;
          }
          public void run(){
            byte[] buffer=new byte[65507];
            while(true){
              if(stopped) return;
              DatagramPacket dp=new DatagramPacket(buffer,buffer.length);
              try{
                socket.receive(dp);
                String s=new String(dp.getData(),0,dp.getLength());
                System.out.println("AAAAAAAAAAAAAAAAAA");
                System.out.println(s);
                System.out.println("***************************");
                Thread.yield();
              }
              catch(IOException e){
                System.err.println(e);
              }
            }
          }
}
          static class SenderThread extends Thread {
                  private InetAddress server;
                  private DatagramSocket socket;
                  private boolean stopped=false;
                  private int port;
                  public SenderThread(InetAddress ia,int port)throws SocketException{
                    this.server =ia;
                    this.socket =new DatagramSocket();
                    this.port =port;
                  }
                  public void halt(){
                    this.stopped =true;
                  }
                  public DatagramSocket getSocket(){
                    return this.socket ;
                  }
                  public void run(){
                      try{
                      System.out.println("BBBBBBBBBBBBBBBBBBBBBBBB");
                      BufferedReader userInput=new BufferedReader(new InputStreamReader(System.in));
                      while(true){
                        if(stopped) return;
                        String longLine="";
                        while(true){
                                String theLine=userInput.readLine();
                                if(theLine.equals("send")) break;
                                longLine = longLine + theLine;
                                longLine = longLine + "+\n";
                        }
                        longLine = longLine + "\n";
                        System.out.println("-------------------");
                        System.out.println(longLine);
                        System.out.println("-------------------");
                        byte[] data=longLine.getBytes();
                        DatagramPacket output=new DatagramPacket(data,data.length,server,port);
                        socket.send(output);
                        Thread.yield();
                      }
                    }
                    catch(IOException e){
                      System.err.println(e);
                    }
                  }
                }

}

