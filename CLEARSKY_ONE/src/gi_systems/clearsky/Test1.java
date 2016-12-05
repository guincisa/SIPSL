//Project Clearsky
//24.11.2016
//V1 Levenshtein

package gi_systems.clearsky;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.GregorianCalendar;

public class Test1 {
	

	
	public static void main(String[] args) throws IOException {
		// TODO Auto-generated method stub

		System.out.println("start");
		String e1 = "   +++ XBEOCS21 2016-11-24 08:10:57    #999999 0-1-6 >\n"+
					"** REPT MANUAL ASSERT=105, SPA=EPAY28E\n"+
					"   Default QoS Range in Diameter Parameter table xxxx be used\n"+
					"::processingErrorAlarm::applicationSubsystemFailure::0-1-6#EPAY28E::Default QoS Range in Diameter Parameter table will be used::\n"+
					"Client ID = 20005\n"+
					"Default QoS Range = 100\n"+
					"Subscriber ID = 44777826051\n"+
					"   END OF REPORT #999999++-";
		String e2 = "   +++ XBEOCS21 2016-11-24 08:10:57    #999999 0-1-6 >\n"+
				"** REPT MANUAL ASSERT=104, SPA=EPAY28E\n"+
				"   Default QoS Range in Diameter Parameter table will be used\n"+
				"::processingErrorAlarm::applicationSubsystemFailure::0-1-6#EPAY28E::Default QoS Range in Diameter Parameter table will be used::\n"+
				"Client ID = 20005\n"+
				"Default QoS Range = 100\n"+
				"Subscriber ID = 44777826051\n"+
				"   END OF REPORT #999999++-";
		String e3 = "   +++ XBEOCS21 2016-11-24 08:11:01 assert   #999999 0-1-13 >\n"+
					"   REPT INTERNAL ASSERT=305, SPA=EPAY28E\n"+
					"   447787626452:PGWBE5:58:947470436:552807358touple_not_found, need reset ppcall\n"+
					"s to clear Glb table\n"+
					"::processingErrorAlarm::applicationSubsystemFailure::0-1-13#EPAY28E::447787626452:PGWBE5:58:947470436:552807358touple_not_found, need reset ppcalls to clear Glb table::\n"+
					"Client ID = 27007\n"+
					"Subscriber\n"+
					"   END OF REPORT #999999++-\n";


		System.out.println(e1);
		System.out.println(e2);
		int L1 = Levensthein.compare(e1,e2);
		int L2 = Levensthein.compare(e1,e3);
		System.out.println(L1);
		System.out.println(L2);
		System.out.println(100*(double)((double)L1/(double)e1.length()));
		System.out.println(100*(double)((double)L2/(double)e1.length()));
		
		//-------------------------------------------------------------------------------------------------------------------------

		//Read OMlog and slipt asserts into blocks
		String BLOCK[] = new String[1500000];
		String ssCurrentLine;
		BufferedReader br = null;
		br = new BufferedReader(new FileReader("C:\\Users\\incisadicameranag\\Documents\\NOC\\CLEARSKY\\LOG1"));
		int i = 0;
		while ((ssCurrentLine = br.readLine()) != null){
			if (ssCurrentLine.endsWith("++-")){
				BLOCK[i]=BLOCK[i]+ssCurrentLine;
				i++;
			}else{
				BLOCK[i]=BLOCK[i]+ssCurrentLine;
			}
		}
		System.out.println("done " + i);
		System.out.println("2\n" + BLOCK[2]);
		SimpleDateFormat formatter = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
		Date date = null;
		Calendar calendar = GregorianCalendar.getInstance();
		
        int k = 0;
		int L3;
		int modulo;
		int minute=0;
		
		System.out.println("------------------------------------\n"+BLOCK[23001]+"\n-------------------------------------");
		for (int ii = 0 ; ii < 1000000; ii ++){
			L3 = Levensthein.compare(BLOCK[23001],BLOCK[ii]);
			//L3 = Levensthein.compare(BLOCK[30],BLOCK[ii]);

			//System.out.println("L3 " + L3);
			if ( L3 < 40){
				if (L3 >35){
					System.out.println("++++++++++++++++++++++++++++++++++++\n"+BLOCK[ii]+"\n+++++++++++++++++++++++++++++++++++++++");
				}
				//System.out.println(BLOCK[ii]);
				for (int is = 0; is<BLOCK[ii].length();is++){
					try {
						date = formatter.parse(BLOCK[ii].substring(is));
					}catch(ParseException pe){
						continue;
					}
					break;
				}
				//System.out.println("Date is " + date.toString());
				calendar.setTime(date);
				calendar.get(Calendar.MINUTE);
				
				modulo = calendar.get(Calendar.MINUTE) % 5;
				if(modulo == 0 && minute != calendar.get(Calendar.MINUTE)){
					minute = calendar.get(Calendar.MINUTE);
					System.out.println(date.toString() + "--->" + k);
					k=0;
				}
				k++;
			}
		}

	}
	

}
