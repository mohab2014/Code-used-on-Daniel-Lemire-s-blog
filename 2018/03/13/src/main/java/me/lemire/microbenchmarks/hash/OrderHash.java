package me.lemire.microbenchmarks.hash;

import java.util.*;
import java.io.*;



import org.openjdk.jmh.annotations.*;
import org.openjdk.jmh.runner.*;
import org.openjdk.jmh.runner.options.*;

import java.util.*;
import java.util.concurrent.*;

@State(Scope.Thread)
@BenchmarkMode(Mode.Throughput)
@OutputTimeUnit(TimeUnit.SECONDS)
public class OrderHash {


   public int N = 1_000_000 ;
   LinkedHashSet<Integer> lhs = new LinkedHashSet<Integer>();
   HashSet<Integer> hs = new HashSet<Integer>();
   int[] lhsdump = new int[N];
   int[] hsdump = new int[N];




   @Setup
   public void setup() {
          ThreadLocalRandom r = ThreadLocalRandom.current();
          for(int k = 0 ; k < N ; k++) {
            int val = r.nextInt();
            lhs.add(val);
            hs.add(val);
          }
          int pos = 0; 
          for(int k : lhs) {
           lhsdump[pos++] = k;
          }
          pos = 0;
          for(int k : hs) {
           hsdump[pos++] = k;
          }
 
    }
    @Benchmark
    public boolean verifyLinkedHashSet() {
      for(int i : lhsdump) 
        if(!lhs.contains(i)) return false;
      return true;
    }
    @Benchmark
    public boolean verifyHashSet() {
      for(int i : hsdump) 
        if(!hs.contains(i)) return false;
      return true;
    }

    @Benchmark
    public int scanLinkedHashSet() {
      int sum = 0;
      for(int i : lhs) 
        sum += i;
      return sum;
    }
    @Benchmark
    public int scanHashSet() {
      int sum = 0;
      for(int i : hs) 
        sum += i;
      return sum;
    }

    public static void main(String[] args) throws RunnerException {
       Options opt = new OptionsBuilder()
                .include(OrderHash.class.getSimpleName())
                .warmupIterations(5)
                .measurementIterations(5)
                .forks(1)
                .build();

        new Runner(opt).run();
    }
}
