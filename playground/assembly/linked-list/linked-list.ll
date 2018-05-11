; ModuleID = 'linked-list.bc'
source_filename = "./sources/linked-list.cpp"
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
@llvm.global_ctors = appending global [1 x { i32, void ()*, i8* }] [{ i32, void ()*, i8* } { i32 65535, void ()* @_GLOBAL__sub_I_linked_list.cpp, i8* null }]

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
define i32 @main() local_unnamed_addr #4 {
entry:
  %retval = alloca i32, align 4
  %nodes = alloca [100 x %struct.list_node], align 16
  %list_it = alloca %struct.list_node*, align 8
  %i = alloca i32, align 4
  %sum = alloca i32, align 4
  store i32 0, i32* %retval, align 4
  %array.begin = getelementptr inbounds [100 x %struct.list_node], [100 x %struct.list_node]* %nodes, i32 0, i32 0
  %arrayctor.end = getelementptr inbounds %struct.list_node, %struct.list_node* %array.begin, i64 100
  br label %arrayctor.loop

arrayctor.loop:                                   ; preds = %arrayctor.loop, %entry
  %arrayctor.cur = phi %struct.list_node* [ %array.begin, %entry ], [ %arrayctor.next, %arrayctor.loop ]
  call void @_ZN9list_nodeC2Ev(%struct.list_node* %arrayctor.cur)
  %arrayctor.next = getelementptr inbounds %struct.list_node, %struct.list_node* %arrayctor.cur, i64 1
  %arrayctor.done = icmp eq %struct.list_node* %arrayctor.next, %arrayctor.end
  br i1 %arrayctor.done, label %arrayctor.cont, label %arrayctor.loop

arrayctor.cont:                                   ; preds = %arrayctor.loop
  store i32 0, i32* %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %arrayctor.cont
  %0 = load i32, i32* %i, align 4
  %cmp = icmp ult i32 %0, 99
  br i1 %cmp, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  %1 = load i32, i32* %i, align 4
  %2 = load i32, i32* %i, align 4
  %idxprom = zext i32 %2 to i64
  %arrayidx = getelementptr inbounds [100 x %struct.list_node], [100 x %struct.list_node]* %nodes, i64 0, i64 %idxprom
  %value = getelementptr inbounds %struct.list_node, %struct.list_node* %arrayidx, i32 0, i32 0
  store i32 %1, i32* %value, align 16
  %3 = load i32, i32* %i, align 4
  %add = add i32 %3, 1
  %idxprom1 = zext i32 %add to i64
  %arrayidx2 = getelementptr inbounds [100 x %struct.list_node], [100 x %struct.list_node]* %nodes, i64 0, i64 %idxprom1
  %4 = load i32, i32* %i, align 4
  %idxprom3 = zext i32 %4 to i64
  %arrayidx4 = getelementptr inbounds [100 x %struct.list_node], [100 x %struct.list_node]* %nodes, i64 0, i64 %idxprom3
  %next = getelementptr inbounds %struct.list_node, %struct.list_node* %arrayidx4, i32 0, i32 1
  store %struct.list_node* %arrayidx2, %struct.list_node** %next, align 8
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %5 = load i32, i32* %i, align 4
  %inc = add i32 %5, 1
  store i32 %inc, i32* %i, align 4
  br label %for.cond

for.end:                                          ; preds = %for.cond
  %6 = load i32, i32* %i, align 4
  %7 = load i32, i32* %i, align 4
  %idxprom5 = zext i32 %7 to i64
  %arrayidx6 = getelementptr inbounds [100 x %struct.list_node], [100 x %struct.list_node]* %nodes, i64 0, i64 %idxprom5
  %value7 = getelementptr inbounds %struct.list_node, %struct.list_node* %arrayidx6, i32 0, i32 0
  store i32 %6, i32* %value7, align 16
  %8 = load i32, i32* %i, align 4
  %idxprom8 = zext i32 %8 to i64
  %arrayidx9 = getelementptr inbounds [100 x %struct.list_node], [100 x %struct.list_node]* %nodes, i64 0, i64 %idxprom8
  %next10 = getelementptr inbounds %struct.list_node, %struct.list_node* %arrayidx9, i32 0, i32 1
  store %struct.list_node* null, %struct.list_node** %next10, align 8
  store i32 0, i32* %sum, align 4
  %arrayidx11 = getelementptr inbounds [100 x %struct.list_node], [100 x %struct.list_node]* %nodes, i64 0, i64 0
  store %struct.list_node* %arrayidx11, %struct.list_node** %list_it, align 8
  br label %while.cond

while.cond:                                       ; preds = %while.body, %for.end
  %9 = load %struct.list_node*, %struct.list_node** %list_it, align 8
  %cmp12 = icmp ne %struct.list_node* %9, null
  br i1 %cmp12, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %10 = load %struct.list_node*, %struct.list_node** %list_it, align 8
  %value13 = getelementptr inbounds %struct.list_node, %struct.list_node* %10, i32 0, i32 0
  %11 = load i32, i32* %value13, align 8
  %12 = load i32, i32* %sum, align 4
  %add14 = add nsw i32 %12, %11
  store i32 %add14, i32* %sum, align 4
  %13 = load %struct.list_node*, %struct.list_node** %list_it, align 8
  %next15 = getelementptr inbounds %struct.list_node, %struct.list_node* %13, i32 0, i32 1
  %14 = load %struct.list_node*, %struct.list_node** %next15, align 8
  store %struct.list_node* %14, %struct.list_node** %list_it, align 8
  br label %while.cond

while.end:                                        ; preds = %while.cond
  %15 = load i32, i32* %sum, align 4
  %call = call dereferenceable(272) %"class.std::basic_ostream"* @_ZNSolsEi(%"class.std::basic_ostream"* @_ZSt4cout, i32 %15)
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

declare dereferenceable(272) %"class.std::basic_ostream"* @_ZNSolsEi(%"class.std::basic_ostream"*, i32) local_unnamed_addr #1

; Function Attrs: noinline uwtable
define internal void @_GLOBAL__sub_I_linked_list.cpp() #0 section ".text.startup" {
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

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 6.0.0 (https://git.llvm.org/git/clang.git/ ff0c0d8ab3e316bb6e2741fedb3b545e198eab7a) (https://git.llvm.org/git/llvm.git/ 089d4c0c490687db6c75f1d074e99c4d42936a50)"}
