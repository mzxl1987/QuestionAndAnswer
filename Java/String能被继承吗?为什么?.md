  # String能被继承吗 ? 为什么?
  * 不可以,因为String类有final修饰符,实现细节不允许改变
  ```
   public final class String
    implements java.io.Serializable, Comparable<String>, CharSequence {
    /** The value is used for character storage. */
    //最终String的数据都存储在 char[] 中
    private final char value[];
  
    /** Cache the hash code for the string */
    private int hash; // Default to 0
  
    /** use serialVersionUID from JDK 1.0.2 for interoperability */
    private static final long serialVersionUID = -6849794470754667710L;
  ```
  * String 一旦创建无法改变,在String的开头的注释中有这么一句话
  ```
  Strings are constant; their values cannot be changed after they are created. String buffers support mutable strings.
  Because String objects are immutable they can be shared. For example:
      String str = "abc";
    is equivalent to:
      char data[] = {'a', 'b', 'c'};
      String str = new String(data);
  ```
  * String构造函数的几种实现与区别
    * 创建一个empty character sequence
    ```
    public String() {
        this.value = "".value;
    }
    ```
    * 创建一个与给定参数相同的字符序列,新创建的String是参数String的拷贝
    ```
    public String(String original) {
        this.value = original.value;
        this.hash = original.hash;
    }
    ```
    * 使用char[] 创建一个String对象, 新的对象同样是char[]的拷贝
    ```
    public String(char value[]) {
        this.value = Arrays.copyOf(value, value.length);
    }
    ```
    Arrays.class
    ```
    public static char[] copyOf(char[] original, int newLength) {
        char[] copy = new char[newLength];
        System.arraycopy(original, 0, copy, 0,
                         Math.min(original.length, newLength));
        return copy;
    }
    ```
