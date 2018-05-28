; ModuleID = 'dep_test.bc'
source_filename = "./sources/dep_test.cpp"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@_ZL1a = internal unnamed_addr global [100 x i32] zeroinitializer, align 16

; Function Attrs: noinline norecurse nounwind optnone uwtable
define i32 @main() local_unnamed_addr #0 {
entry:
  %retval = alloca i32, align 4
  %i = alloca i32, align 4
  %i5 = alloca i32, align 4
  %j = alloca i32, align 4
  %i22 = alloca i32, align 4
  %j26 = alloca i32, align 4
  %k = alloca i32, align 4
  %j51 = alloca i32, align 4
  store i32 0, i32* %retval, align 4
  store i32 0, i32* %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  %0 = load i32, i32* %i, align 4
  %cmp = icmp ult i32 %0, 100
  br i1 %cmp, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  %1 = load i32, i32* %i, align 4
  %idxprom = zext i32 %1 to i64
  %arrayidx = getelementptr inbounds [100 x i32], [100 x i32]* @_ZL1a, i64 0, i64 %idxprom
  %2 = load i32, i32* %arrayidx, align 4
  %3 = load i32, i32* %i, align 4
  %idxprom1 = zext i32 %3 to i64
  %add = add i32 %2, 0
  %4 = load i32, i32* %i, align 4
  %idxprom3 = zext i32 %4 to i64
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %5 = load i32, i32* %i, align 4
  %inc = add i32 %5, 1
  store i32 %inc, i32* %i, align 4
  br label %for.cond

for.end:                                          ; preds = %for.cond
  store i32 0, i32* %i5, align 4
  br label %for.cond6

for.cond6:                                        ; preds = %for.inc19, %for.end
  %6 = load i32, i32* %i5, align 4
  %cmp7 = icmp ult i32 %6, 100
  br i1 %cmp7, label %for.body8, label %for.end21

for.body8:                                        ; preds = %for.cond6
  store i32 0, i32* %j, align 4
  br label %for.cond9

for.cond9:                                        ; preds = %for.inc16, %for.body8
  %7 = load i32, i32* %i5, align 4
  %cmp10 = icmp ult i32 %7, 100
  br i1 %cmp10, label %for.body11, label %for.end18

for.body11:                                       ; preds = %for.cond9
  %8 = load i32, i32* %i5, align 4
  %idxprom12 = zext i32 %8 to i64
  %9 = load i32, i32* %j, align 4
  %idxprom14 = zext i32 %9 to i64
  br label %for.inc16

for.inc16:                                        ; preds = %for.body11
  %10 = load i32, i32* %i5, align 4
  %inc17 = add i32 %10, 1
  store i32 %inc17, i32* %i5, align 4
  br label %for.cond9

for.end18:                                        ; preds = %for.cond9
  br label %for.inc19

for.inc19:                                        ; preds = %for.end18
  %11 = load i32, i32* %i5, align 4
  %inc20 = add i32 %11, 1
  store i32 %inc20, i32* %i5, align 4
  br label %for.cond6

for.end21:                                        ; preds = %for.cond6
  store i32 0, i32* %i22, align 4
  br label %for.cond23

for.cond23:                                       ; preds = %for.inc64, %for.end21
  %12 = load i32, i32* %i22, align 4
  %cmp24 = icmp ult i32 %12, 100
  br i1 %cmp24, label %for.body25, label %for.end66

for.body25:                                       ; preds = %for.cond23
  store i32 0, i32* %j26, align 4
  br label %for.cond27

for.cond27:                                       ; preds = %for.inc48, %for.body25
  %13 = load i32, i32* %i22, align 4
  %cmp28 = icmp ult i32 %13, 100
  br i1 %cmp28, label %for.body29, label %for.end50

for.body29:                                       ; preds = %for.cond27
  store i32 0, i32* %k, align 4
  br label %for.cond30

for.cond30:                                       ; preds = %for.inc45, %for.body29
  %14 = load i32, i32* %i22, align 4
  %cmp31 = icmp ult i32 %14, 100
  br i1 %cmp31, label %for.body32, label %for.end47

for.body32:                                       ; preds = %for.cond30
  %15 = load i32, i32* %i22, align 4
  %idxprom33 = zext i32 %15 to i64
  %16 = load i32, i32* %k, align 4
  %idxprom35 = zext i32 %16 to i64
  %17 = load i32, i32* %k, align 4
  %idxprom37 = zext i32 %17 to i64
  %18 = load i32, i32* %j26, align 4
  %idxprom39 = zext i32 %18 to i64
  %add41 = add i32 0, 0
  %19 = load i32, i32* %j26, align 4
  %idxprom42 = zext i32 %19 to i64
  %arrayidx43 = getelementptr inbounds [100 x i32], [100 x i32]* @_ZL1a, i64 0, i64 %idxprom42
  %20 = load i32, i32* %arrayidx43, align 4
  %add44 = add i32 %20, %add41
  store i32 %add44, i32* %arrayidx43, align 4
  br label %for.inc45

for.inc45:                                        ; preds = %for.body32
  %21 = load i32, i32* %i22, align 4
  %inc46 = add i32 %21, 1
  store i32 %inc46, i32* %i22, align 4
  br label %for.cond30

for.end47:                                        ; preds = %for.cond30
  br label %for.inc48

for.inc48:                                        ; preds = %for.end47
  %22 = load i32, i32* %i22, align 4
  %inc49 = add i32 %22, 1
  store i32 %inc49, i32* %i22, align 4
  br label %for.cond27

for.end50:                                        ; preds = %for.cond27
  store i32 0, i32* %j51, align 4
  br label %for.cond52

for.cond52:                                       ; preds = %for.inc61, %for.end50
  %23 = load i32, i32* %i22, align 4
  %cmp53 = icmp ult i32 %23, 100
  br i1 %cmp53, label %for.body54, label %for.end63

for.body54:                                       ; preds = %for.cond52
  %24 = load i32, i32* %j51, align 4
  %idxprom55 = zext i32 %24 to i64
  %arrayidx56 = getelementptr inbounds [100 x i32], [100 x i32]* @_ZL1a, i64 0, i64 %idxprom55
  %25 = load i32, i32* %arrayidx56, align 4
  %26 = load i32, i32* %i22, align 4
  %idxprom57 = zext i32 %26 to i64
  %27 = load i32, i32* %j51, align 4
  %idxprom59 = zext i32 %27 to i64
  br label %for.inc61

for.inc61:                                        ; preds = %for.body54
  %28 = load i32, i32* %i22, align 4
  %inc62 = add i32 %28, 1
  store i32 %inc62, i32* %i22, align 4
  br label %for.cond52

for.end63:                                        ; preds = %for.cond52
  br label %for.inc64

for.inc64:                                        ; preds = %for.end63
  %29 = load i32, i32* %i22, align 4
  %inc65 = add i32 %29, 1
  store i32 %inc65, i32* %i22, align 4
  br label %for.cond23

for.end66:                                        ; preds = %for.cond23
  ret i32 0
}

attributes #0 = { noinline norecurse nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 6.0.0 (https://git.llvm.org/git/clang.git/ ff0c0d8ab3e316bb6e2741fedb3b545e198eab7a) (https://git.llvm.org/git/llvm.git/ 089d4c0c490687db6c75f1d074e99c4d42936a50)"}
