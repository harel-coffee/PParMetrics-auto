; ModuleID = 'linked-list-traversal.bc'
source_filename = "./sources/linked-list-traversal.cpp"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%"class.std::ios_base::Init" = type { i8 }
%"class.std::basic_ostream" = type { i32 (...)**, %"class.std::basic_ios" }
%"class.std::basic_ios" = type { %"class.std::ios_base", %"class.std::basic_ostream"*, i8, i8, %"class.std::basic_streambuf"*, %"class.std::ctype"*, %"class.std::num_put"*, %"class.std::num_get"* }
%"class.std::ios_base" = type { i32 (...)**, i64, i64, i32, i32, i32, %"struct.std::ios_base::_Callback_list"*, %"struct.std::ios_base::_Words", [8 x %"struct.std::ios_base::_Words"], i32, %"struct.std::ios_base::_Words"*, %"class.std::locale" }
%"struct.std::ios_base::_Callback_list" = type { %"struct.std::ios_base::_Callback_list"*, void (i32, %"class.std::ios_base"*, i32)*, i32, i32 }
%"struct.std::ios_base::_Words" = type { i8*, i64 }
%"class.std::locale" = type { %"class.std::locale::_Impl"* }
%"class.std::locale::_Impl" = type { i32, %"class.std::locale::facet"**, i64, %"class.std::locale::facet"**, i8** }
%"class.std::locale::facet" = type <{ i32 (...)**, i32, [4 x i8] }>
%"class.std::basic_streambuf" = type { i32 (...)**, i8*, i8*, i8*, i8*, i8*, i8*, %"class.std::locale" }
%"class.std::ctype" = type <{ %"class.std::locale::facet.base", [4 x i8], %struct.__locale_struct*, i8, [7 x i8], i32*, i32*, i16*, i8, [256 x i8], [256 x i8], i8, [6 x i8] }>
%"class.std::locale::facet.base" = type <{ i32 (...)**, i32 }>
%struct.__locale_struct = type { [13 x %struct.__locale_data*], i16*, i32*, i32*, [13 x i8*] }
%struct.__locale_data = type opaque
%"class.std::num_put" = type { %"class.std::locale::facet.base", [4 x i8] }
%"class.std::num_get" = type { %"class.std::locale::facet.base", [4 x i8] }
%struct.list_node = type { i32, %struct.list_node* }

$_ZN9list_nodeC2Ev = comdat any

@_ZStL8__ioinit = internal global %"class.std::ios_base::Init" zeroinitializer, align 1
@__dso_handle = external hidden global i8
@_ZSt4cout = external global %"class.std::basic_ostream", align 8
@llvm.global_ctors = appending global [1 x { i32, void ()*, i8* }] [{ i32, void ()*, i8* } { i32 65535, void ()* @_GLOBAL__sub_I_linked_list_traversal.cpp, i8* null }]

; Function Attrs: noinline uwtable
define internal fastcc void @__cxx_global_var_init() unnamed_addr #0 section ".text.startup" {
  tail call void @_ZNSt8ios_base4InitC1Ev(%"class.std::ios_base::Init"* nonnull @_ZStL8__ioinit)
  %1 = tail call i32 @__cxa_atexit(void (i8*)* bitcast (void (%"class.std::ios_base::Init"*)* @_ZNSt8ios_base4InitD1Ev to void (i8*)*), i8* getelementptr inbounds (%"class.std::ios_base::Init", %"class.std::ios_base::Init"* @_ZStL8__ioinit, i64 0, i32 0), i8* nonnull @__dso_handle) #3
  ret void
}

declare void @_ZNSt8ios_base4InitC1Ev(%"class.std::ios_base::Init"*) unnamed_addr #1

; Function Attrs: nounwind
declare void @_ZNSt8ios_base4InitD1Ev(%"class.std::ios_base::Init"*) unnamed_addr #2

; Function Attrs: nounwind
declare i32 @__cxa_atexit(void (i8*)*, i8*, i8*) local_unnamed_addr #3

; Function Attrs: noinline norecurse optnone uwtable
define i32 @main() local_unnamed_addr #4 personality i8* bitcast (i32 (...)* @__gxx_personality_v0 to i8*) {
  %1 = alloca i32, align 4
  %2 = alloca %struct.list_node, align 8
  %3 = alloca %struct.list_node*, align 8
  %4 = alloca i32, align 4
  %5 = alloca i8*
  %6 = alloca i32
  %7 = alloca i32, align 4
  %8 = alloca %struct.list_node*, align 8
  store i32 0, i32* %1, align 4
  call void @_ZN9list_nodeC2Ev(%struct.list_node* %2)
  store %struct.list_node* %2, %struct.list_node** %3, align 8
  store i32 1, i32* %4, align 4
  br label %9

; <label>:9:                                      ; preds = %24, %0
  %10 = load i32, i32* %4, align 4
  %11 = icmp ult i32 %10, 100
  br i1 %11, label %12, label %27

; <label>:12:                                     ; preds = %9
  %13 = call i8* @_Znwm(i64 16) #8
  %14 = bitcast i8* %13 to %struct.list_node*
  call void @_ZN9list_nodeC2Ev(%struct.list_node* %14)
  br label %15

; <label>:15:                                     ; preds = %12
  %16 = load %struct.list_node*, %struct.list_node** %3, align 8
  %17 = getelementptr inbounds %struct.list_node, %struct.list_node* %16, i32 0, i32 1
  store %struct.list_node* %14, %struct.list_node** %17, align 8
  %18 = load i32, i32* %4, align 4
  %19 = load %struct.list_node*, %struct.list_node** %3, align 8
  %20 = getelementptr inbounds %struct.list_node, %struct.list_node* %19, i32 0, i32 0
  store i32 %18, i32* %20, align 8
  %21 = load %struct.list_node*, %struct.list_node** %3, align 8
  %22 = getelementptr inbounds %struct.list_node, %struct.list_node* %21, i32 0, i32 1
  %23 = load %struct.list_node*, %struct.list_node** %22, align 8
  store %struct.list_node* %23, %struct.list_node** %3, align 8
  br label %24

; <label>:24:                                     ; preds = %15
  %25 = load i32, i32* %4, align 4
  %26 = add i32 %25, 1
  store i32 %26, i32* %4, align 4
  br label %9

; <label>:27:                                     ; preds = %9
  store i32 0, i32* %7, align 4
  store %struct.list_node* %2, %struct.list_node** %3, align 8
  br label %28

; <label>:28:                                     ; preds = %31, %27
  %29 = load %struct.list_node*, %struct.list_node** %3, align 8
  %30 = icmp ne %struct.list_node* %29, null
  br i1 %30, label %31, label %40

; <label>:31:                                     ; preds = %28
  %32 = load %struct.list_node*, %struct.list_node** %3, align 8
  %33 = getelementptr inbounds %struct.list_node, %struct.list_node* %32, i32 0, i32 0
  %34 = load i32, i32* %33, align 8
  %35 = load i32, i32* %7, align 4
  %36 = add nsw i32 %35, %34
  store i32 %36, i32* %7, align 4
  %37 = load %struct.list_node*, %struct.list_node** %3, align 8
  %38 = getelementptr inbounds %struct.list_node, %struct.list_node* %37, i32 0, i32 1
  %39 = load %struct.list_node*, %struct.list_node** %38, align 8
  store %struct.list_node* %39, %struct.list_node** %3, align 8
  br label %28

; <label>:40:                                     ; preds = %28
  store %struct.list_node* %2, %struct.list_node** %3, align 8
  br label %41

; <label>:41:                                     ; preds = %59, %40
  %42 = load %struct.list_node*, %struct.list_node** %3, align 8
  %43 = getelementptr inbounds %struct.list_node, %struct.list_node* %42, i32 0, i32 1
  %44 = load %struct.list_node*, %struct.list_node** %43, align 8
  %45 = icmp ne %struct.list_node* %44, null
  br i1 %45, label %46, label %60

; <label>:46:                                     ; preds = %41
  %47 = load %struct.list_node*, %struct.list_node** %3, align 8
  store %struct.list_node* %47, %struct.list_node** %8, align 8
  %48 = load %struct.list_node*, %struct.list_node** %3, align 8
  %49 = getelementptr inbounds %struct.list_node, %struct.list_node* %48, i32 0, i32 1
  %50 = load %struct.list_node*, %struct.list_node** %49, align 8
  store %struct.list_node* %50, %struct.list_node** %3, align 8
  %51 = load %struct.list_node*, %struct.list_node** %8, align 8
  %52 = icmp ne %struct.list_node* %51, %2
  br i1 %52, label %53, label %59

; <label>:53:                                     ; preds = %46
  %54 = load %struct.list_node*, %struct.list_node** %8, align 8
  %55 = icmp eq %struct.list_node* %54, null
  br i1 %55, label %58, label %56

; <label>:56:                                     ; preds = %53
  %57 = bitcast %struct.list_node* %54 to i8*
  call void @_ZdlPv(i8* %57) #9
  br label %58

; <label>:58:                                     ; preds = %56, %53
  br label %59

; <label>:59:                                     ; preds = %58, %46
  br label %41

; <label>:60:                                     ; preds = %41
  %61 = load %struct.list_node*, %struct.list_node** %3, align 8
  %62 = icmp eq %struct.list_node* %61, null
  br i1 %62, label %65, label %63

; <label>:63:                                     ; preds = %60
  %64 = bitcast %struct.list_node* %61 to i8*
  call void @_ZdlPv(i8* %64) #9
  br label %65

; <label>:65:                                     ; preds = %63, %60
  %66 = load i32, i32* %7, align 4
  %67 = call dereferenceable(272) %"class.std::basic_ostream"* @_ZNSolsEi(%"class.std::basic_ostream"* @_ZSt4cout, i32 %66)
  ret i32 0
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr void @_ZN9list_nodeC2Ev(%struct.list_node*) unnamed_addr #5 comdat align 2 {
  %2 = alloca %struct.list_node*, align 8
  store %struct.list_node* %0, %struct.list_node** %2, align 8
  %3 = load %struct.list_node*, %struct.list_node** %2, align 8
  %4 = getelementptr inbounds %struct.list_node, %struct.list_node* %3, i32 0, i32 0
  store i32 0, i32* %4, align 8
  %5 = getelementptr inbounds %struct.list_node, %struct.list_node* %3, i32 0, i32 1
  store %struct.list_node* null, %struct.list_node** %5, align 8
  ret void
}

; Function Attrs: nobuiltin
declare noalias nonnull i8* @_Znwm(i64) local_unnamed_addr #6

declare i32 @__gxx_personality_v0(...)

; Function Attrs: nobuiltin nounwind
declare void @_ZdlPv(i8*) local_unnamed_addr #7

declare dereferenceable(272) %"class.std::basic_ostream"* @_ZNSolsEi(%"class.std::basic_ostream"*, i32) local_unnamed_addr #1

; Function Attrs: noinline uwtable
define internal void @_GLOBAL__sub_I_linked_list_traversal.cpp() #0 section ".text.startup" {
  tail call fastcc void @__cxx_global_var_init()
  ret void
}

attributes #0 = { noinline uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { nounwind }
attributes #4 = { noinline norecurse optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #5 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #6 = { nobuiltin "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #7 = { nobuiltin nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #8 = { builtin }
attributes #9 = { builtin nounwind }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 6.0.0 (https://git.llvm.org/git/clang.git/ 75a36832b158dc653cbf481c4c6ae05f4acbbff8) (https://github.com/llvm-mirror/llvm.git 138d74f09c10f1f47b37659de24cde3939f95873)"}
