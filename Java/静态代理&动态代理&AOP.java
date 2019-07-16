import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Method;
import java.lang.reflect.Proxy;

public class Test {
	
    public static void main(String[] args) {
    	
    	//静态代理
    	Base a = new BaseImplProxy(new BaseImpl());
    	a.add();
    	
    	//动态代理
    	LogHandler lh = new LogHandler();
    	Base da = (Base)lh.newLogHandler(new BaseImpl());
    	da.add();
    	
    	Base log = (Base)lh.newLogHandler(new LogImpl());
    	log.add();
    	
    }
    
    
    
    
}


/**
 * 接口
 * @author Administrator
 *
 */
interface Base {
	void add();
}

/**
 * 实现类
 * @author Administrator
 *
 */
class BaseImpl implements Base{
	@Override
	public void add() {
		System.out.println("BaseImpl.add()");
	}
}

/**
 * 实现类
 * @author Administrator
 *
 */
class LogImpl implements Base{
	@Override
	public void add() {
		System.out.println("LogImpl.add()");
	}
}


/**
 * 静态代理
 * 优点: 通过Proxy类隐藏了代理类的实现
 * 缺点：1.当委托来interface添加新的方法的时候，相应的代理类也需要添加新的方法实现
 *         会出现大量重复的代码
 *       2.只能服务于一个类型的对象,如果出现新的对象则需要添加新的代理
 * @author Administrator
 *
 */

/**
 * 代理类/工厂类
 * @author Administrator
 *
 */
class BaseImplProxy implements Base{
	
	Base a;
	
	public BaseImplProxy(BaseImpl b){
		this.a = b;
	}
	
	@Override
	public void add() {
		System.out.println("BaseImplProxy.add() before");
		a.add();
		System.out.println("BaseImplProxy.add() after");
	}
	
}

/**
 * 动态代理
 * 优点：动态代理与静态代理相比较，
 *       接口中声明的所有方法都被转移到调用处理器一个集中的方法中处理（InvocationHandler.invoke）
                  在接口方法数量比较多的时候，我们可以进行灵活处理，
                  而不需要像静态代理那样每一个方法进行中转。
                  而且动态代理的应用使我们的类职责更加单一，复用性更强
   AOP(面向切面编程)的基本思想就是基于动态代理实现
 **/
class LogHandler implements InvocationHandler{

	private Object target;
	
	public Object newLogHandler(Object src){
		this.target = src;
		return Proxy.newProxyInstance(target.getClass().getClassLoader(), target.getClass().getInterfaces(),this);
	}
	
	/* (non-Javadoc)
	 * @see java.lang.reflect.InvocationHandler#invoke(java.lang.Object, java.lang.reflect.Method, java.lang.Object[])
	 */
	@Override
	public Object invoke(Object proxy, Method method, Object[] args) throws Throwable {
		
		System.out.println(method);
		System.out.println("参数");
		if(args != null){
			for (Object object : args) {
				System.out.println(object);
			}
		}
		
		System.out.println("执行方法,before");
		Object result = method.invoke(target, args);
		System.out.println("执行方法,after");
		
		return result;
	}
	
}

