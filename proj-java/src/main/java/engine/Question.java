package engine;

import java.time.LocalDate;

import java.util.Set;
import java.util.HashSet;
import java.util.List;
import java.util.ArrayList;

/**
 * Classe que guarda a informacao de uma resposta.
 */
public class Question extends Post{

    private String title;
    private Set<String> tags;
    private List<Long> answers;

    public String toString(){
        StringBuilder sb = new StringBuilder("");
        sb.append(super.toString()).append("\n");
        sb.append(title).append("\n");
        return sb.toString();
    }

    /**
     * Construtor de Pergunta Default
     */
    public Question(){
        super();
        this.title = "n/a";
        this.tags = new HashSet<>();
        this.answers = new ArrayList<>();
    }

    /**
     * Construtor de Pergunta cópia
     *
     * @param  q 	Pergunta a ser copiada
     */
    public Question(Question q){
        super(q);
        setTitle(q.getTitle());
        setTags(q.getTags());
        setAnswers(q.getAnswers());
    }

    /**
     * Construtor de Pergunta parametrizado para comparacao por data, usado em sets ordenados
     *
     * @param  id 	Id da pergunta
     * @param  d 	Data de criacao
     */
    public Question(long id, LocalDate d){
        super(id, d, 0);
    }

    /**
     * Construtor de Pergunta parametrizado
     *
     * @param  title 	Titlo
     * @param  tag 		Set com as tags da pergunta
     * @param  ans 		Lista com os ids das respostas correspondentes à pergunta
     * @param  id 		Id da pergunta
     * @param  date 	Data de criacao da pergunta
     * @param  ownerId 	Id do autor
     */
    public Question(String title, Set<String> tag, List<Long> ans, long id, LocalDate date, long ownerId){
        super(id, date, ownerId);
        setTitle(title);
        setTags(tag);
        setAnswers(ans);
    }

    /**
    * Retorna um objeto menor que todas as perguntas com data de criacao posterior ou igual a d
    * @param d data do objeto
    */
    static public Question fromDate(LocalDate d){
        return new Question(Integer.MIN_VALUE, d);
    }

    /**
    * Retorna um objeto maior que todas as perguntas com data de criacao anterior ou igual a d
    * @param d data do objeto
    */
    static public Question toDate(LocalDate d){
        return new Question(Integer.MAX_VALUE, d);
    }


    /**
     * Retorna o titulo da Pergunta
     *
     * @return  Titulo da pergunta
     */
    public String getTitle(){
        return this.title;
    }

    /**
     * Retorna a colecao de Tags associadas a Pergunta
     *
     * @return Colecao com as tags da pergunta
     */
    public Set<String> getTags(){
        return new HashSet<>(this.tags);
    }

    /**
     * Retorna uma Lista com os ids das respostas associadas a Pergunta
     *
     * @return  Lista com os ids
     */
    public List<Long> getAnswers(){
        return new ArrayList<>(this.answers);
    }

    /**
     * Define o titulo da Pergunta
     *
     * @param  t 	Novo titulo da pergunta
     */
    public void setTitle(String t){
        this.title = t;
    }

    /**
     * Define a colecao de Tags associadas a Pergunta
     *
     * @param  t 	Nova Colecao de tags da pergunta
     */
    public void setTags(Set<String> t){
        this.tags = new HashSet<>(t);
    }

    /**
     * Define a lista de Respostas associadas à Pergunta
     *
     * @param  a 	Nova lista de ids de respostas
     */
    public void setAnswers(List<Long> a){
        this.answers = new ArrayList<>(a);
    }

    /**
     * Metodo clone da Pergunta
     *
     * @return Question clone
     */
    public Question clone(){
        return new Question(this);
    }

    /**
     * Adiciona uma Resposta associada a Pergunta
     *
     * @param  a 	Id da resposta a ser adicionada
     */
    public void addAnswer(long a){
        this.answers.add(a);
    }

}
