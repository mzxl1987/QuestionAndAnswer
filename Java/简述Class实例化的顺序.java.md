# 简述Class实例化的顺序
比如父类静态数据，构造函数，字段，子类静态数据，构造函数，字段，当 new 的时候， 他们的执行顺序
* ### 实例化的顺序如下
  * #### 静态代码
    * ##### 父类变量  static int p = 0;
    * ##### 父类静态代码块  static { p = 2; }
    * ##### 子类变量  static int s = 0
    * ##### 子类静态代码块  static { s = 2; }
  * #### 非静态代码
    * ##### 父类非静态成员变量 
    * ##### 父类构造函数
    * ##### 子类非静态成员变量 
    * ##### 子类构造函数
* ### 测试代码如下
```
public class Test {

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		
		new son();
		
	}
	
	
	
}

class parent{
    private static  int a=1;
    private static  int b;
    private   int c=initc();
    static {
        b=1;
        System.out.println("1.父类静态代码块：赋值b成功");
        System.out.println("1.父类静态代码块：a的值"+a);
    }
    int initc(){
        System.out.println("3.父类成员变量赋值：---> c的值"+c);
        this.c=12;
        System.out.println("3.父类成员变量赋值：---> c的值"+c);
        return c;
    }
    public parent(){
        System.out.println("4.父类构造方式开始执行---> a:"+a+",b:"+b);
        System.out.println("4.父类构造方式开始执行---> c:"+c);
    }
}

class son extends parent{
    private static  int sa=1;
    private static  int sb;
    private   int sc=initc2();
    static {
        sb=1;
        System.out.println("2.子类静态代码块：赋值sb成功");
        System.out.println("2.子类静态代码块：sa的值"+sa);
    }
    int initc2(){
        System.out.println("5.子类成员变量赋值--->：sc的值"+sc);
        this.sc=12;
        return sc;
    }
    public son(){
        System.out.println("6.子类构造方式开始执行---> sa:"+sa+",sb:"+sb);
        System.out.println("6.子类构造方式开始执行---> sc:"+sc);
    }
}
```
输出结果:
```
1.父类静态代码块：赋值b成功
1.父类静态代码块：a的值1
2.子类静态代码块：赋值sb成功
2.子类静态代码块：sa的值1
3.父类成员变量赋值：---> c的值0
3.父类成员变量赋值：---> c的值12
4.父类构造方式开始执行---> a:1,b:1
4.父类构造方式开始执行---> c:12
5.子类成员变量赋值--->：sc的值0
6.子类构造方式开始执行---> sa:1,sb:1
6.子类构造方式开始执行---> sc:12
```
