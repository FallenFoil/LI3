package engine;

import java.time.LocalDate;

import java.util.Comparator;

/**
 * Classe que guarda a informacao de uma pergunta.
 */
public class Answer extends Post{

    private int score;
    private int commentCount;
    private long questionId;

    /**
     * Construtor de Resposta Default
     */
    public Answer(){
      super();
      this.score = 0;
      this.commentCount = 0;
      this.questionId = 0;

    }

    /**
     * Construtor de Resposta parametrizado para comparacao por data, usado em sets ordenados
     *
     * @param  id      Id da resposta
     * @param  d       Data de criacao
     */
    private Answer(long id, LocalDate d){
        super(id, d, -1);
    }

    /**
     * Construtor parametrizado de Answer;
     *
     * @param  score        Score da resposta
     * @param  commCount    Numero de comentarios
     * @param  questionId   Id da pergunta correspondente
     * @param  id           Id da resposta
     * @param  date         Data da resposta
     * @param  ownerId      Id do autor
     */

    public Answer(int score, int commCount, long questionId, long id, LocalDate date, long ownerId){
      super(id, date, ownerId);
      this.score = score;
      this.commentCount = commCount;
      this.questionId = questionId;
    }

    /**
     * Construtor de Resposta cópia
     *
     * @param  a    Resposta a ser copiada
     */
    public Answer(Answer a){
      super(a);
      this.score = a.getScore();
      this.commentCount = a.getCommentCount();
      this.questionId = a.getQuestion();
    }

    /**
     * Obter o Score da resposta;
     *
     * @return  Score da resposta
     */
    public int getScore(){
        return this.score;
    }

    /**
     * Obter o número de comentários da resposta;
     *
     * @return  Numero de comentarios de uma resposta
     */
    public int getCommentCount(){
        return this.commentCount;
    }

    /**
     * Obter o id da pergunta associado à resposta;
     *
     * @return  Id da pergunta correspondente
     */
    public long getQuestion(){
      return this.questionId;
    }

    /**
     * Definir o score da resposta;
     *
     * @param  s    Novo score da resposta
     */
    public void setScore(int s){
        this.score = s;
    }

    /**
     * Definir o número de comentários da resposta;
     *
     * @param  c    Novo numero de comentarios da respota
     */
    public void setCommentCount(int c){
        this.commentCount = c;
    }

    /**
     * Definir o id da pergunta associada à resposta;
     *
     * @param  q     Novo id da pergunta correspondente
     */
    public void setQuestion(long q){
        this.questionId = q;
    }

    /**
     * Retorna um objeto menor que todas as respostas com data posterior ou igual a d
     *
     * @param d data do objeto
     */
    static public Answer fromDate(LocalDate d){
        return new Answer(Integer.MIN_VALUE, d);
    }

    /**
     * Retorna um objeto maior que todas as respostas com data anterior ou igual a d
     *
     * @param d data do objeto
     */
    static public Answer toDate(LocalDate d){
        return new Answer(Integer.MAX_VALUE, d);
    }
    
    public static class compareScore implements Comparator<Answer> {
        public int compare(Answer a, Answer b){
            return Integer.compare(b.getScore(), a.getScore());
        }
    }


    /**
     * Metodo que transforma a informacao da resposta num String
     *
     * @return  String com a informacao da resposta
     */
    public String toString(){
        StringBuilder sb = new StringBuilder("");
        sb.append(super.toString()).append("\n");
        sb.append("score: ").append(score).append("\n");
        sb.append("Com Count: ").append(commentCount).append("\n");
        sb.append("ParentId: ").append(questionId).append("\n");
        return sb.toString();
    }

    /**
     * Metodo clone de uma Answer
     *
     * @return  Answer;
     */
    public Answer clone(){
        return (new Answer(this));
    }
}
