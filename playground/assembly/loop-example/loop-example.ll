; ModuleID = 'loop-example.bc'
source_filename = "./sources/loop-example.cpp"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%"class.std::ios_base::Init" = type { i8 }
%struct.list_node = type { i32, %struct.list_node* }

$_ZN9list_nodeC2Ev = comdat any

@_ZStL8__ioinit = internal global %"class.std::ios_base::Init" zeroinitializer, align 1
@__dso_handle = external hidden global i8
@llvm.global_ctors = appending global [1 x { i32, void ()*, i8* }] [{ i32, void ()*, i8* } { i32 65535, void ()* @_GLOBAL__sub_I_loop_example.cpp, i8* null }]

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
  store i32 0, i32* %retval, align 4
  call void @_ZN9list_nodeC2Ev(%struct.list_node* %begin)
  store i32 1, i32* %i, align 4
  store %struct.list_node* %begin, %struct.list_node** %list_it, align 8
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  %0 = load i32, i32* %i, align 4
  %cmp = icmp ult i32 %0, 100
  br i1 %cmp, label %land.rhs, label %land.end

land.rhs:                                         ; preds = %for.cond
  %1 = load %struct.list_node*, %struct.list_node** %list_it, align 8
  %cmp1 = icmp ne %struct.list_node* %1, null
  br label %land.end

land.end:                                         ; preds = %land.rhs, %for.cond
  %2 = phi i1 [ false, %for.cond ], [ %cmp1, %land.rhs ]
  br i1 %2, label %for.body, label %for.end

for.body:                                         ; preds = %land.end
  %call = call i8* @_Znwm(i64 16) #7
  %3 = bitcast i8* %call to %struct.list_node*
  call void @_ZN9list_nodeC2Ev(%struct.list_node* %3)
  br label %invoke.cont

invoke.cont:                                      ; preds = %for.body
  %4 = load %struct.list_node*, %struct.list_node** %list_it, align 8
  %next = getelementptr inbounds %struct.list_node, %struct.list_node* %4, i32 0, i32 1
  store %struct.list_node* %3, %struct.list_node** %next, align 8
  %5 = load i32, i32* %i, align 4
  %6 = load %struct.list_node*, %struct.list_node** %list_it, align 8
  %value = getelementptr inbounds %struct.list_node, %struct.list_node* %6, i32 0, i32 0
  store i32 %5, i32* %value, align 8
  br label %for.inc

for.inc:                                          ; preds = %invoke.cont
  %7 = load %struct.list_node*, %struct.list_node** %list_it, align 8
  %next2 = getelementptr inbounds %struct.list_node, %struct.list_node* %7, i32 0, i32 1
  %8 = load %struct.list_node*, %struct.list_node** %next2, align 8
  store %struct.list_node* %8, %struct.list_node** %list_it, align 8
  %9 = load i32, i32* %i, align 4
  %inc = add i32 %9, 1
  store i32 %inc, i32* %i, align 4
  br label %for.cond

for.end:                                          ; preds = %land.end
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

; Function Attrs: noinline uwtable
define internal void @_GLOBAL__sub_I_loop_example.cpp() #0 section ".text.startup" {
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
attributes #7 = { builtin }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 6.0.0 (https://git.llvm.org/git/clang.git/ ff0c0d8ab3e316bb6e2741fedb3b545e198eab7a) (https://git.llvm.org/git/llvm.git/ 089d4c0c490687db6c75f1d074e99c4d42936a50)"}
