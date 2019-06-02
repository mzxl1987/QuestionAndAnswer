# 简述
Java中定义了java.util.Map的接口,该接口实现了常用的4个类:HashMap,TreeMap,HashTable,LinkedHashMap.
特点如下:
###### HashMap
> HashMap:根据Key的hashCode存储数据,大多数情况下可以直接定位到数据,又很快的访问速度,但是便利书序不确定.
> HashMap中的Key不可以重复,只允许一个key的值为null,但是value可以重复,可以有多个null.
> HashMap不是线程安全的,所以多线程访问的时候需要用Collections.synchronizedMap(Map())实现线程安全,或者使用ConcurrentHashMap替代
###### HashTable
> Hashtable功能上与HashMap类型，HashTable继承自Dictionary类.
> HashTable是线程安全的.相应的方法都用synchronized做了修饰,但是在效率上没有ConcurrentHashMap更高效,ConcurrentHashMap使用分段锁.

###### LinkedHashMap 
> LinkedHashMap是HashMap的一个子类,是一个双向链表,Node<>默认插入到尾部,所以用iterator访问的时候访问的顺序就是存放的顺序.
###### TreeMap
> TreeMap 实现了 SortedMap接口, 将按照键排序进行升序排放,可以指定比较器.使用iterator遍历的结果是按照键排序后的结果.
> 如果使用排序的映射的时候，建议使用TreeMap. Treemap.Key必须实现Comparable接口,或者new TreeMap()的时候在构造函数中传入自定义的Comparable
> 否则会跑出ClassCastException
上述Map类型中要求Key是不可改变对象,不可改变是指该对象的HashCode不会改变.如果HashCode改变了,Map对象就定位不到映射的对象了
# 存储结构



