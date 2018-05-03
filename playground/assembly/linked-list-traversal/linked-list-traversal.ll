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
entry:
  tail call void @_ZNSt8ios_base4InitC1Ev(%"class.std::ios_base::Init"* nonnull @_ZStL8__ioinit)
  %0 = tail call i32 @__cxa_atexit(void (i8*)* bitcast (void (%"class.std::ios_base::Init"*)* @_ZNSt8ios_base4InitD1Ev to void (i8*)*), i8* getelementptr inbounds (%"class.std::ios_base::Init", %"class.std::ios_base::Init"* @_ZStL8__ioinit, i64 0, i32 0), i8* nonnull @__dso_handle) #3
  ret void
}

declare void @_ZNSt8ios_base4InitC1Ev(%"class.std::ios_base::Init"*) unnamed_addr #1

; Function Attrs: nounwind
declare void @_ZNSt8ios_base4InitD1Ev(%"class.std::ios_base::Init"*) unnamed_addr #2

; Function Attrs: nounwind
declare i32 @__cxa_atexit(void (i8*)*, i8*, i8*) local_unnamed_addr #3

; Function Attrs: noinline norecurse optnone uwtable
define i32 @main() local_unnamed_addr #4 personality i8* bitcast (i32 (...)* @__gxx_personality_v0 to i8*) {
entry:
  %retval = alloca i32, align 4
  %begin = alloca %struct.list_node, align 8
  %list_it = alloca %struct.list_node*, align 8
  %i = alloca i32, align 4
  %exn.slot = alloca i8*
  %ehselector.slot = alloca i32
  %sum = alloca i32, align 4
  %prev_it = alloca %struct.list_node*, align 8
  store i32 0, i32* %retval, align 4
  call void @_ZN9list_nodeC2Ev(%struct.list_node* %begin)
  store %struct.list_node* %begin, %struct.list_node** %list_it, align 8
  store i32 1, i32* %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  %0 = load i32, i32* %i, align 4
  %cmp = icmp ult i32 %0, 100
  br i1 %cmp, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  %call = call i8* @_Znwm(i64 16) #8
  %1 = bitcast i8* %call to %struct.list_node*
  call void @_ZN9list_nodeC2Ev(%struct.list_node* %1)
  br label %invoke.cont

invoke.cont:                                      ; preds = %for.body
  %2 = load %struct.list_node*, %struct.list_node** %list_it, align 8
  %next = getelementptr inbounds %struct.list_node, %struct.list_node* %2, i32 0, i32 1
  store %struct.list_node* %1, %struct.list_node** %next, align 8
  %3 = load i32, i32* %i, align 4
  %4 = load %struct.list_node*, %struct.list_node** %list_it, align 8
  %value = getelementptr inbounds %struct.list_node, %struct.list_node* %4, i32 0, i32 0
  store i32 %3, i32* %value, align 8
  %5 = load %struct.list_node*, %struct.list_node** %list_it, align 8
  %next1 = getelementptr inbounds %struct.list_node, %struct.list_node* %5, i32 0, i32 1
  %6 = load %struct.list_node*, %struct.list_node** %next1, align 8
  store %struct.list_node* %6, %struct.list_node** %list_it, align 8
  br label %for.inc

for.inc:                                          ; preds = %invoke.cont
  %7 = load i32, i32* %i, align 4
  %inc = add i32 %7, 1
  store i32 %inc, i32* %i, align 4
  br label %for.cond

for.end:                                          ; preds = %for.cond
  store i32 0, i32* %sum, align 4
  store %struct.list_node* %begin, %struct.list_node** %list_it, align 8
  br label %while.cond

while.cond:                                       ; preds = %while.body, %for.end
  %8 = load %struct.list_node*, %struct.list_node** %list_it, align 8
  %cmp2 = icmp ne %struct.list_node* %8, null
  br i1 %cmp2, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %9 = load %struct.list_node*, %struct.list_node** %list_it, align 8
  %value3 = getelementptr inbounds %struct.list_node, %struct.list_node* %9, i32 0, i32 0
  %10 = load i32, i32* %value3, align 8
  %11 = load i32, i32* %sum, align 4
  %add = add nsw i32 %11, %10
  store i32 %add, i32* %sum, align 4
  %12 = load %struct.list_node*, %struct.list_node** %list_it, align 8
  %next4 = getelementptr inbounds %struct.list_node, %struct.list_node* %12, i32 0, i32 1
  %13 = load %struct.list_node*, %struct.list_node** %next4, align 8
  store %struct.list_node* %13, %struct.list_node** %list_it, align 8
  br label %while.cond

while.end:                                        ; preds = %while.cond
  store %struct.list_node* %begin, %struct.list_node** %list_it, align 8
  br label %while.cond5

while.cond5:                                      ; preds = %if.end, %while.end
  %14 = load %struct.list_node*, %struct.list_node** %list_it, align 8
  %next6 = getelementptr inbounds %struct.list_node, %struct.list_node* %14, i32 0, i32 1
  %15 = load %struct.list_node*, %struct.list_node** %next6, align 8
  %cmp7 = icmp ne %struct.list_node* %15, null
  br i1 %cmp7, label %while.body8, label %while.end11

while.body8:                                      ; preds = %while.cond5
  %16 = load %struct.list_node*, %struct.list_node** %list_it, align 8
  store %struct.list_node* %16, %struct.list_node** %prev_it, align 8
  %17 = load %struct.list_node*, %struct.list_node** %list_it, align 8
  %next9 = getelementptr inbounds %struct.list_node, %struct.list_node* %17, i32 0, i32 1
  %18 = load %struct.list_node*, %struct.list_node** %next9, align 8
  store %struct.list_node* %18, %struct.list_node** %list_it, align 8
  %19 = load %struct.list_node*, %struct.list_node** %prev_it, align 8
  %cmp10 = icmp ne %struct.list_node* %19, %begin
  br i1 %cmp10, label %if.then, label %if.end

if.then:                                          ; preds = %while.body8
  %20 = load %struct.list_node*, %struct.list_node** %prev_it, align 8
  %isnull = icmp eq %struct.list_node* %20, null
  br i1 %isnull, label %delete.end, label %delete.notnull

delete.notnull:                                   ; preds = %if.then
  %21 = bitcast %struct.list_node* %20 to i8*
  call void @_ZdlPv(i8* %21) #9
  br label %delete.end

delete.end:                                       ; preds = %delete.notnull, %if.then
  br label %if.end

if.end:                                           ; preds = %delete.end, %while.body8
  br label %while.cond5

while.end11:                                      ; preds = %while.cond5
  %22 = load %struct.list_node*, %struct.list_node** %list_it, align 8
  %isnull12 = icmp eq %struct.list_node* %22, null
  br i1 %isnull12, label %delete.end14, label %delete.notnull13

delete.notnull13:                                 ; preds = %while.end11
  %23 = bitcast %struct.list_node* %22 to i8*
  call void @_ZdlPv(i8* %23) #9
  br label %delete.end14

delete.end14:                                     ; preds = %delete.notnull13, %while.end11
  %24 = load i32, i32* %sum, align 4
  %call15 = call dereferenceable(272) %"class.std::basic_ostream"* @_ZNSolsEi(%"class.std::basic_ostream"* @_ZSt4cout, i32 %24)
  ret i32 0
}

; Function Attrs: noinline nounwind optnone uwtable
define linkonce_odr void @_ZN9list_nodeC2Ev(%struct.list_node* %this) unnamed_addr #5 comdat align 2 {
entry:
  %this.addr = alloca %struct.list_node*, align 8
  store %struct.list_node* %this, %struct.list_node** %this.addr, align 8
  %this1 = load %struct.list_node*, %struct.list_node** %this.addr, align 8
  %value = getelementptr inbounds %struct.list_node, %struct.list_node* %this1, i32 0, i32 0
  store i32 0, i32* %value, align 8
  %next = getelementptr inbounds %struct.list_node, %struct.list_node* %this1, i32 0, i32 1
  store %struct.list_node* null, %struct.list_node** %next, align 8
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
entry:
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
!1 = !{!"clang version 6.0.0 (https://git.llvm.org/git/clang.git/ ff0c0d8ab3e316bb6e2741fedb3b545e198eab7a) (https://git.llvm.org/git/llvm.git/ 089d4c0c490687db6c75f1d074e99c4d42936a50)"}
