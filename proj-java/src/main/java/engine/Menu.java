package engine;

import java.time.format.DateTimeParseException;
import java.time.LocalDate;
import java.util.Collections;
import java.util.List;
import java.util.ArrayList;
import java.util.Map;
import java.util.HashMap;
import java.util.Set;
import java.util.TreeSet;
import java.util.HashSet;
import java.util.Iterator;
import java.util.stream.Collectors;
import java.util.Scanner;
import java.util.InputMismatchException;
import java.io.File;
import java.io.IOException;
import li3.TADCommunity;
import engine.TCDCommunity;
import common.MyLog;
import common.Pair;
import parser.ParseUsers;
import parser.ParsePosts;
import parser.ParseTags;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;
import org.xml.sax.SAXException;



/** Class Menu
* Terminal User Interface
*/

public class Menu{

        private Scanner s;
        private TADCommunity dmp;


    /** Construtor de Class Default
    * Inicialização do Scanner
    */

    public Menu(){
	   this.s = new Scanner(System.in);
    }

    /** Metodo de execução do menu
    * 12 Operações
    */

	private void menu(){
        int op = -1;

        do {
            System.out.println("-----------------------------------------------------");
            System.out.println("---------------Terminal-User-Interface---------------");
            System.out.println("-----------------------------------------------------");
            System.out.println("1.  Query 1: Informação de um Post");
            System.out.println("2.  Query 2: Utilizadores com mais Posts");
            System.out.println("3.  Query 3: Número de Respostas e Perguntas");
            System.out.println("4.  Query 4: Perguntas com certa Tag");
            System.out.println("5.  Query 5: Bio do Utilizador e Ultimos Posts");
            System.out.println("6.  Query 6: Respostas com melhor Score");
            System.out.println("7.  Query 7: Perguntas com mais Respostas");
            System.out.println("8.  Query 8: Verifica uma palavra nos Titulos");
            System.out.println("9.  Query 9: Ultimas Perguntas de 2 Utilizadores");
            System.out.println("10. Query 10: Melhor Resposta");
            System.out.println("11. Query 11: Tags mais usadas");
            System.out.println("0.  Sair\n-----------------------------------------------------\n-----------------------------------------------------");

            try {
                op = s.nextInt();
                switch(op){
                	case 0: break;
                	case 1: query1(); break;
                	case 2: query2(); break;
                	case 3: query3(); break;
                	case 4: query4(); break;
                    
                	case 5: query5(); break;
                	case 6: query6(); break;
                	case 7: query7(); break;
                	case 8: query8(); break;
                	case 9: query9(); break;
                	case 10: query10(); break;
                	case 11: query11(); break;
                    
                	default: System.out.println("Introduza um número entre 0 e 11"); break;
                }
            } 
            catch (NumberFormatException e){
                System.out.println("Input Inválido");
                op = -1;
            }


        } while(op != 0);
        s.close();
	}

    /** Operação 1
    * Query 1
    */

	private void query1(){
		long id;
		do{
			System.out.println("Insira o Id do Post");

			try {
				id = (s.nextLong());
				Pair<String,String> query1 = dmp.infoFromPost(id);
				System.out.println("-----------------------------------------------------\n" +
                    "-----------------------------------------------------\nTitulo: " + query1.getFst());
				System.out.println("Autor: " + query1.getSnd());
				break;
			}
			catch (InputMismatchException e){
				System.out.println("Input Inválido");
			}
		} while(true);
	}

    /** Query 2
    * Pede ao utilizador o número de utilizadores que pretende visualizar
    * Imprime os Ids dos utilizadores mais ativos
    */

	private void query2(){

		do{
			System.out.println("Insira o número de utilizadores que prentende visualizar");

			try {
				int n = (s.nextInt());
				List<Long> query2 = dmp.topMostActive(n);
                System.out.println("-----------------------------------------------------\n" +
                    "-----------------------------------------------------\nIds dos Utilizadores:");
				for(long l : query2){
                    System.out.println(l);
                }  
                break;
			}
            catch(InputMismatchException e){
                System.out.println("Input Inválido");
            }
		} while(true);
	}

    /** 
    *Metodo que obtém as datas através de input do utilizador
    *@return Pair<LocalDate, LocalDate>
    */

    private Pair<LocalDate, LocalDate> getQueryDate(){

        LocalDate di = LocalDate.parse("2000-01-02");
        LocalDate df = LocalDate.parse("2000-01-01");

        do{
            do{
                System.out.println("Insira a data inicial no formato YYYY-MM-DD");
            
                try{
                    di = LocalDate.parse(s.next());
                    break;
                }   
                catch(DateTimeParseException e){
                    System.out.println("Input Inválido");
                }
            } while(true);

            do{
                System.out.println("Insira a data final no formato YYYY-MM-DD");
            
                try{
                    df = LocalDate.parse(s.next());
                    break;
                }
                catch(DateTimeParseException e){
                    System.out.println("Input Inválido");
                }
            } while(true);

            if(df.isBefore(di)) System.out.println("A data final é anterior à data inicial");

        } while (df.isBefore(di));

        return (new Pair(di,df));
    }


    /** Query 3
    * Pede ao utilizador a data inicial e final
    * Imprime o número de respostas e de perguntas no intervalo de tempo
    */
    private void query3(){

        Pair<LocalDate,LocalDate> dates = getQueryDate();
        Pair<Long,Long> query3 = dmp.totalPosts(dates.getFst(), dates.getSnd());
        System.out.println("-----------------------------------------------------" +
            "\n-----------------------------------------------------\n" +
            "No intervalo de tempo " + dates.getFst() + " ," + dates.getSnd()
         + " existem " + query3.getFst() + " perguntas e "
         + query3.getSnd() + " respostas");

    }

    /** Query 4
    * Pede ao utilizador uma tag
    * Pede ao utilizador a data inicial e final
    * Imprime os Ids das perguntas com a respetiva tag no intervalo de tempo
    */

    private void query4(){

        System.out.println("Insira a tag");

        String tag = s.next();

        Pair<LocalDate,LocalDate> dates = getQueryDate();

        List<Long> query4 = dmp.questionsWithTag(tag, dates.getFst(), dates.getSnd());
        System.out.println("-----------------------------------------------------"+
            "\n-----------------------------------------------------\n" +
            "Ids das Perguntas com a tag " + tag + ":");
        for(long l : query4){
            System.out.println(l);
        }
    }


    /** Query5
    * Pede o id do utilizador
    * Imprime a Bio do utilizador e os ids dos seus ultimos Posts
    */

    private void query5(){
        
        do{
            System.out.println("Insira o Id do Utilizador");

            try {
                long id = (s.nextLong());
                Pair<String, List<Long>> query5 = dmp.getUserInfo(id);
                System.out.println("-----------------------------------------------------"+
            "\n-----------------------------------------------------\n" +
            "Bio do Utilizador:");
                System.out.println(query5.getFst());
                System.out.println("Ids dos Posts:");
                List<Long> lst = query5.getSnd();
                for(long l: lst) System.out.println(l);
                break;
            }
            catch (InputMismatchException e){
                System.out.println("Input Inválido");
            }
        } while(true);
    }

    /** Query 6
    * Pede o número de respostas que pretende visualizar
    * Pede ao utilizador a data inicial e final
    * Imprime os Ids das Respostas com melhor votação
    */

    private void query6(){
        int n;
        do{
            System.out.println("Insira número de respostas que quer visualizar");

            try {
                n = (s.nextInt());
                break;
            }
            catch (InputMismatchException e){
                System.out.println("Input Inválido");
            }
        } while(true);

        Pair<LocalDate,LocalDate> dates = getQueryDate();
        List<Long> query6 = dmp.mostVotedAnswers(n, dates.getFst(), dates.getSnd());
        System.out.println("-----------------------------------------------------"+
            "\n-----------------------------------------------------\n" +
            "Ids Das Respostas:");
        for(long l : query6) 
            System.out.println(l);
    }

    /** Query 7
    * Pede o número de Perguntas que pretende visualizar
    * Pede ao utilizador a data inicial e final
    * Imprime os Ids das Perguntas com mais respostas
    */

    private void query7(){
        int n;
        do{
            System.out.println("Insira número de Perguntas que quer visualizar");

            try {
                n = (s.nextInt());
                break;
            }
            catch (InputMismatchException e){
                System.out.println("Input Inválido");
            }
        } while(true);

        Pair<LocalDate,LocalDate> dates = getQueryDate();
        List<Long> query7 = dmp.mostAnsweredQuestions(n, dates.getFst(), dates.getSnd());
        System.out.println("-----------------------------------------------------"+
            "\n-----------------------------------------------------\n" +
            "Ids Das Perguntas:");
        for(long l : query7)
            System.out.println(l);
    }

    /** Query 8
    * Pede o número de Perguntas que pretende visualizar
    * Pede a palavra que pretende verificar
    * Imprime os Ids das Perguntas que contem a palavra no titulo
    */

    private void query8(){
        int n;
        do{
            System.out.println("Insira número de Perguntas que quer visualizar");

            try {
                n = (s.nextInt());
                break;
            }
            catch (InputMismatchException e){
                System.out.println("Input Inválido");
            }
        } while(true);

        String word;
        do{
            System.out.println("Insira a palavra");

            try {
                word = (s.next());
                break;
            }
            catch (InputMismatchException e){
                System.out.println("Input Inválido");
            }
        } while(true);

        List<Long> query8 = dmp.containsWord(n, word);
        System.out.println("-----------------------------------------------------"+
            "\n-----------------------------------------------------\n" +
            "Ids Das Perguntas que contem " + word + " no titulo:");
        for(long l : query8) 
            System.out.println(l);
    }

    /** Query 9
    * Pede os Ids dos 2 utilizadores
    * Pede o número de Posts que pretende visualizar
    * Imprime os Ids das Perguntas que ambos os utilizadores participaram
    */
    private void query9(){

        long u1;
        do{
            System.out.println("Insira o Id do primeiro utilizador");

            try {
                u1 = (s.nextLong());
                break;
            }
            catch (InputMismatchException e){
                System.out.println("Input Inválido");
            }
        } while(true);

        long u2;
        do{
            System.out.println("Insira o Id do segundo utilizador");

            try {
                u2 = (s.nextLong());
                break;
            }
            catch (InputMismatchException e){
                System.out.println("Input Inválido");
            }
        } while(true);

        int n;
        do{
            System.out.println("Insira o número de Posts que pretende visualizar");

            try {
                n = (s.nextInt());
                break;
            }
            catch (InputMismatchException e){
                System.out.println("Input Inválido");
            }
        } while(true);
        System.out.println("-----------------------------------------------------"+
            "\n-----------------------------------------------------\n" +
            "Ids Das Perguntas em que os dois utilizadores participaram:");
        List<Long> query9 = dmp.bothParticipated(n, u1, u2);
        for(long l : query9)
            System.out.println(l);

    }

    /** Query 10
    * Pede o Id da Pergunta
    * Imprime a melhor Resposta associada a Pergunta
    */

    private void query10(){
        long p;
        do{
            System.out.println("Insira o Id da pergunta");

            try {
                p = (s.nextLong());
                break;
            }
            catch (InputMismatchException e){
                System.out.println("Input Inválido");
            }
        } while(true);

        System.out.println("-----------------------------------------------------"+
            "\n-----------------------------------------------------\n" +
            "Id da melhor resposta:" + dmp.betterAnswer(p));
    }


    /** Query 11
    * Pede o número de tags
    * Pede ao utilizador a data inicial e final
    * Imprime as Tags mais usadas pelos utilizadores com melhor reputação
    */

    private void query11(){
                
        int n;
        do{
            System.out.println("Insira o número de tags");

            try {
                n = (s.nextInt());
                break;
            }
            catch (InputMismatchException e){
                System.out.println("Input Inválido");
            }
        } while(true);

        Pair<LocalDate,LocalDate> dates = getQueryDate();

        List<Long> query11 = dmp.mostUsedBestRep(n, dates.getFst(), dates.getSnd());
        
        System.out.println("-----------------------------------------------------"+
            "\n-----------------------------------------------------\n" +
            "Id das Tags mais usadas");

        for(long l : query11)
            System.out.println(l);

    }

    /** Metodo de Inicialização do Menu
    *@param TADCommunity
    */
    
    public void run(TADCommunity dmp){
        this.dmp = dmp;
        this.menu();
    }
}

