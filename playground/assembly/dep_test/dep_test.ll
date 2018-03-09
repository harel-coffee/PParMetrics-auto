; ModuleID = 'dep_test.bc'
source_filename = "./sources/dep_test.cpp"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@_ZL1a = internal unnamed_addr global [100 x i32] zeroinitializer, align 16

; Function Attrs: noinline norecurse nounwind optnone uwtable
define i32 @main() local_unnamed_addr #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  %8 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  store i32 0, i32* %2, align 4
  br label %9

; <label>:9:                                      ; preds = %22, %0
  %10 = load i32, i32* %2, align 4
  %11 = icmp ult i32 %10, 100
  br i1 %11, label %12, label %25

; <label>:12:                                     ; preds = %9
  %13 = load i32, i32* %2, align 4
  %14 = zext i32 %13 to i64
  %15 = getelementptr inbounds [100 x i32], [100 x i32]* @_ZL1a, i64 0, i64 %14
  %16 = load i32, i32* %15, align 4
  %17 = load i32, i32* %2, align 4
  %18 = zext i32 %17 to i64
  %19 = add i32 %16, 0
  %20 = load i32, i32* %2, align 4
  %21 = zext i32 %20 to i64
  br label %22

; <label>:22:                                     ; preds = %12
  %23 = load i32, i32* %2, align 4
  %24 = add i32 %23, 1
  store i32 %24, i32* %2, align 4
  br label %9

; <label>:25:                                     ; preds = %9
  store i32 0, i32* %3, align 4
  br label %26

; <label>:26:                                     ; preds = %42, %25
  %27 = load i32, i32* %3, align 4
  %28 = icmp ult i32 %27, 100
  br i1 %28, label %29, label %45

; <label>:29:                                     ; preds = %26
  store i32 0, i32* %4, align 4
  br label %30

; <label>:30:                                     ; preds = %38, %29
  %31 = load i32, i32* %3, align 4
  %32 = icmp ult i32 %31, 100
  br i1 %32, label %33, label %41

; <label>:33:                                     ; preds = %30
  %34 = load i32, i32* %3, align 4
  %35 = zext i32 %34 to i64
  %36 = load i32, i32* %4, align 4
  %37 = zext i32 %36 to i64
  br label %38

; <label>:38:                                     ; preds = %33
  %39 = load i32, i32* %3, align 4
  %40 = add i32 %39, 1
  store i32 %40, i32* %3, align 4
  br label %30

; <label>:41:                                     ; preds = %30
  br label %42

; <label>:42:                                     ; preds = %41
  %43 = load i32, i32* %3, align 4
  %44 = add i32 %43, 1
  store i32 %44, i32* %3, align 4
  br label %26

; <label>:45:                                     ; preds = %26
  store i32 0, i32* %5, align 4
  br label %46

; <label>:46:                                     ; preds = %96, %45
  %47 = load i32, i32* %5, align 4
  %48 = icmp ult i32 %47, 100
  br i1 %48, label %49, label %99

; <label>:49:                                     ; preds = %46
  store i32 0, i32* %6, align 4
  br label %50

; <label>:50:                                     ; preds = %76, %49
  %51 = load i32, i32* %5, align 4
  %52 = icmp ult i32 %51, 100
  br i1 %52, label %53, label %79

; <label>:53:                                     ; preds = %50
  store i32 0, i32* %7, align 4
  br label %54

; <label>:54:                                     ; preds = %72, %53
  %55 = load i32, i32* %5, align 4
  %56 = icmp ult i32 %55, 100
  br i1 %56, label %57, label %75

; <label>:57:                                     ; preds = %54
  %58 = load i32, i32* %5, align 4
  %59 = zext i32 %58 to i64
  %60 = load i32, i32* %7, align 4
  %61 = zext i32 %60 to i64
  %62 = load i32, i32* %7, align 4
  %63 = zext i32 %62 to i64
  %64 = load i32, i32* %6, align 4
  %65 = zext i32 %64 to i64
  %66 = add i32 0, 0
  %67 = load i32, i32* %6, align 4
  %68 = zext i32 %67 to i64
  %69 = getelementptr inbounds [100 x i32], [100 x i32]* @_ZL1a, i64 0, i64 %68
  %70 = load i32, i32* %69, align 4
  %71 = add i32 %70, %66
  store i32 %71, i32* %69, align 4
  br label %72

; <label>:72:                                     ; preds = %57
  %73 = load i32, i32* %5, align 4
  %74 = add i32 %73, 1
  store i32 %74, i32* %5, align 4
  br label %54

; <label>:75:                                     ; preds = %54
  br label %76

; <label>:76:                                     ; preds = %75
  %77 = load i32, i32* %5, align 4
  %78 = add i32 %77, 1
  store i32 %78, i32* %5, align 4
  br label %50

; <label>:79:                                     ; preds = %50
  store i32 0, i32* %8, align 4
  br label %80

; <label>:80:                                     ; preds = %92, %79
  %81 = load i32, i32* %5, align 4
  %82 = icmp ult i32 %81, 100
  br i1 %82, label %83, label %95

; <label>:83:                                     ; preds = %80
  %84 = load i32, i32* %8, align 4
  %85 = zext i32 %84 to i64
  %86 = getelementptr inbounds [100 x i32], [100 x i32]* @_ZL1a, i64 0, i64 %85
  %87 = load i32, i32* %86, align 4
  %88 = load i32, i32* %5, align 4
  %89 = zext i32 %88 to i64
  %90 = load i32, i32* %8, align 4
  %91 = zext i32 %90 to i64
  br label %92

; <label>:92:                                     ; preds = %83
  %93 = load i32, i32* %5, align 4
  %94 = add i32 %93, 1
  store i32 %94, i32* %5, align 4
  br label %80

; <label>:95:                                     ; preds = %80
  br label %96

; <label>:96:                                     ; preds = %95
  %97 = load i32, i32* %5, align 4
  %98 = add i32 %97, 1
  store i32 %98, i32* %5, align 4
  br label %46

; <label>:99:                                     ; preds = %46
  ret i32 0
}

attributes #0 = { noinline norecurse nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 6.0.0 (https://git.llvm.org/git/clang.git/ 75a36832b158dc653cbf481c4c6ae05f4acbbff8) (https://github.com/llvm-mirror/llvm.git 138d74f09c10f1f47b37659de24cde3939f95873)"}
