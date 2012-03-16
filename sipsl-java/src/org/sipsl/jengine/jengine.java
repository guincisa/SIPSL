package org.sipsl.jengine;

public class jengine {

	
	/**
	 * @param args
	 */
	public static void main(String[] args) {
        System.out.println("main");
        
        engine myeng = new engine();
        
        for (int i = 0; i <20 ; i++){
        	String m = "message" + i;
        	System.out.println("p_w " + m);
        	myeng.p_w(m);
        	try {
				Thread.sleep(1000);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
        	System.out.println("sent " + m);

        }
        try {
			Thread.sleep(10000);
		} catch (InterruptedException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}

        for (int i = 0; i <20 ; i++){
        	String m = "message" + i;
        	System.out.println("p_w " + m);
        	myeng.p_w(m);
        	try {
				Thread.sleep(1000);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
        	System.out.println("sent " + m);

        }

	}

}
